"""
Database module for tracking decompilation progress.

This uses SQLite (zero setup required) to store:
- Which functions exist in the codebase
- Their current status (todo, in_progress, decompiled, etc.)
- Metadata like file paths, modules, difficulty scores
"""

import sqlite3
import os
from pathlib import Path
from datetime import datetime
from typing import List, Dict, Optional


class DecompDatabase:
    """Simple SQLite database for tracking decompilation functions."""
    
    def __init__(self, db_path: str = "functions.db"):
        """Initialize database connection and create tables if needed."""
        self.db_path = db_path
        self.conn = sqlite3.connect(db_path)
        self.conn.row_factory = sqlite3.Row  # Return rows as dicts
        self._create_tables()
        self._migrate()
    
    def _create_tables(self):
        """Create the database schema."""
        cursor = self.conn.cursor()
        
        # Main functions table
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS functions (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT UNIQUE NOT NULL,
                c_file_path TEXT,
                asm_file_path TEXT,
                module TEXT,
                status TEXT DEFAULT 'todo',
                difficulty_score REAL,
                line_count INTEGER,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                last_attempt_at TIMESTAMP,
                attempt_count INTEGER DEFAULT 0,
                notes TEXT
            )
        """)
        
        # Index for faster queries
        cursor.execute("""
            CREATE INDEX IF NOT EXISTS idx_status 
            ON functions(status)
        """)
        cursor.execute("""
            CREATE INDEX IF NOT EXISTS idx_module 
            ON functions(module)
        """)
        cursor.execute("""
            CREATE INDEX IF NOT EXISTS idx_difficulty
            ON functions(difficulty_score)
        """)
        
        # Attempt log (for tracking decompilation tries)
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS attempt_log (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                function_id INTEGER,
                attempt_type TEXT,
                success BOOLEAN,
                error_message TEXT,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (function_id) REFERENCES functions(id)
            )
        """)
        
        # Struct patterns table (for learning types across decomps)
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS struct_patterns (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                symbol_name TEXT UNIQUE NOT NULL,
                likely_type TEXT,
                base_type TEXT,
                confidence REAL DEFAULT 0.0,
                field_map TEXT,
                seen_count INTEGER DEFAULT 1,
                successful_uses INTEGER DEFAULT 0,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        """)
        
        cursor.execute("""
            CREATE INDEX IF NOT EXISTS idx_struct_symbol 
            ON struct_patterns(symbol_name)
        """)
        
        cursor.execute("""
            CREATE INDEX IF NOT EXISTS idx_struct_confidence 
            ON struct_patterns(confidence DESC)
        """)
        
        # Symbol usage tracking (which functions use which symbols)
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS symbol_usage (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                function_name TEXT NOT NULL,
                symbol_name TEXT NOT NULL,
                access_pattern TEXT,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (function_name) REFERENCES functions(name)
            )
        """)
        
        cursor.execute("""
            CREATE INDEX IF NOT EXISTS idx_symbol_usage 
            ON symbol_usage(symbol_name)
        """)
        
        self.conn.commit()
    
    def remove_functions_not_in_set(self, c_file_path: str, valid_names: set):
        """
        Delete all functions from c_file_path whose names are not in valid_names.
        Used to prune entries that fall below a sentinel comment in the source file.
        """
        cursor = self.conn.cursor()
        cursor.execute(
            "SELECT name FROM functions WHERE c_file_path = ?", (c_file_path,)
        )
        to_remove = [row[0] for row in cursor.fetchall() if row[0] not in valid_names]
        if to_remove:
            cursor.executemany(
                "DELETE FROM functions WHERE name = ?", [(n,) for n in to_remove]
            )
            self.conn.commit()
        return to_remove

    def add_function(self, name: str, c_file_path: str, asm_file_path: str, 
                     module: str, line_count: int = 0) -> int:
        """Add a new function to the database."""
        cursor = self.conn.cursor()
        try:
            cursor.execute("""
                INSERT INTO functions 
                (name, c_file_path, asm_file_path, module, line_count)
                VALUES (?, ?, ?, ?, ?)
            """, (name, c_file_path, asm_file_path, module, line_count))
            self.conn.commit()
            return cursor.lastrowid
        except sqlite3.IntegrityError:
            # Function already exists, update it instead
            cursor.execute("""
                UPDATE functions 
                SET c_file_path = ?, asm_file_path = ?, module = ?, 
                    line_count = ?, updated_at = CURRENT_TIMESTAMP
                WHERE name = ?
            """, (c_file_path, asm_file_path, module, line_count, name))
            self.conn.commit()
            cursor.execute("SELECT id FROM functions WHERE name = ?", (name,))
            return cursor.fetchone()[0]
    
    def get_function(self, name: str) -> Optional[Dict]:
        """Get a single function by name."""
        cursor = self.conn.cursor()
        cursor.execute("SELECT * FROM functions WHERE name = ?", (name,))
        row = cursor.fetchone()
        return dict(row) if row else None
    
    def update_status(self, name: str, status: str, notes: str = None):
        """Update the status of a function (does NOT increment attempt_count)."""
        cursor = self.conn.cursor()
        cursor.execute("""
            UPDATE functions 
            SET status = ?, notes = ?, updated_at = CURRENT_TIMESTAMP
            WHERE name = ?
        """, (status, notes, name))
        self.conn.commit()
    
    def record_decompilation_attempt(self, name: str, status: str, notes: str = None):
        """Record an actual decompilation attempt (increments attempt_count)."""
        cursor = self.conn.cursor()
        cursor.execute("""
            UPDATE functions 
            SET status = ?, notes = ?, updated_at = CURRENT_TIMESTAMP,
                last_attempt_at = CURRENT_TIMESTAMP,
                attempt_count = attempt_count + 1
            WHERE name = ?
        """, (status, notes, name))
        self.conn.commit()
    
    def update_function_status(self, name: str, status: str, notes: str = None):
        """Alias for update_status for compatibility."""
        self.update_status(name, status, notes)
    
    def get_functions_by_status(self, status: str, limit: Optional[int] = None, 
                                module: Optional[str] = None, 
                                order_by: str = 'name') -> List[Dict]:
        """
        Get functions with a specific status.
        
        Args:
            status: Status to filter by
            limit: Maximum number of results (None = all)
            module: Optional module filter
            order_by: Field to sort by (default: 'name')
        """
        cursor = self.conn.cursor()
        
        query = "SELECT * FROM functions WHERE status = ?"
        params = [status]
        
        if module:
            query += " AND module = ?"
            params.append(module)
        
        # Determine order
        if order_by == 'line_count':
            query += " ORDER BY line_count ASC, name ASC"
        elif order_by == 'difficulty_score':
            query += " ORDER BY difficulty_score ASC, line_count ASC, name ASC"
        else:
            query += f" ORDER BY {order_by} ASC"
        
        if limit:
            query += " LIMIT ?"
            params.append(limit)
        
        cursor.execute(query, params)
        return [dict(row) for row in cursor.fetchall()]
    
    def get_functions_by_module(self, module: str) -> List[Dict]:
        """Get all functions in a specific module."""
        cursor = self.conn.cursor()
        cursor.execute("""
            SELECT * FROM functions 
            WHERE module = ? 
            ORDER BY status, name
        """, (module,))
        return [dict(row) for row in cursor.fetchall()]
    
    def get_statistics(self) -> Dict:
        """Get overall statistics about decompilation progress."""
        cursor = self.conn.cursor()
        
        # Total counts by status
        cursor.execute("""
            SELECT status, COUNT(*) as count 
            FROM functions 
            GROUP BY status
        """)
        status_counts = {row['status']: row['count'] for row in cursor.fetchall()}
        
        # Total count
        cursor.execute("SELECT COUNT(*) as total FROM functions")
        total = cursor.fetchone()['total']
        
        # Counts by module
        cursor.execute("""
            SELECT module, COUNT(*) as count 
            FROM functions 
            GROUP BY module
            ORDER BY count DESC
        """)
        module_counts = {row['module']: row['count'] for row in cursor.fetchall()}
        
        return {
            'total': total,
            'total_functions': total,  # Keep both for compatibility
            'by_status': status_counts,
            'by_module': module_counts
        }
    
    def get_next_todo(self, limit: int = 10) -> List[Dict]:
        """Get the next functions to work on (todo status, ordered by difficulty).

        Functions with a rank score (from ``mako.sh rank``) come first, sorted
        easiest-to-hardest.  Functions without a score fall back to line_count.
        """
        cursor = self.conn.cursor()
        cursor.execute("""
            SELECT * FROM functions
            WHERE status = 'todo'
            ORDER BY
                CASE WHEN difficulty_score IS NULL THEN 1 ELSE 0 END ASC,
                difficulty_score ASC,
                line_count ASC,
                name ASC
            LIMIT ?
        """, (limit,))
        return [dict(row) for row in cursor.fetchall()]

    def update_rank_scores(self, scores: Dict[str, float]) -> int:
        """Bulk-update difficulty_score from mako.sh rank output.

        Args:
            scores: Mapping of function_name → rank score (0.0 easiest, 1.0 hardest).

        Returns:
            Number of rows updated.
        """
        if not scores:
            return 0
        cursor = self.conn.cursor()
        updated = 0
        for name, score in scores.items():
            cursor.execute(
                "UPDATE functions SET difficulty_score = ?, updated_at = CURRENT_TIMESTAMP "
                "WHERE name = ?",
                (score, name),
            )
            updated += cursor.rowcount
        self.conn.commit()
        return updated

    def _migrate(self):
        """Apply any schema migrations needed for existing databases."""
        cursor = self.conn.cursor()
        # Ensure difficulty_score column exists (older DBs may pre-date it).
        cursor.execute("PRAGMA table_info(functions)")
        cols = {row['name'] for row in cursor.fetchall()}
        if 'difficulty_score' not in cols:
            cursor.execute("ALTER TABLE functions ADD COLUMN difficulty_score REAL")
            self.conn.commit()

    def close(self):
        """Close database connection."""
        self.conn.close()
    
    # ============================================================
    # Struct Learning Methods
    # ============================================================
    
    def record_struct_usage(self, function_name: str, symbol_name: str, 
                           access_pattern: str = None):
        """Record that a function uses a particular symbol/struct."""
        cursor = self.conn.cursor()
        cursor.execute("""
            INSERT INTO symbol_usage (function_name, symbol_name, access_pattern)
            VALUES (?, ?, ?)
        """, (function_name, symbol_name, access_pattern))
        self.conn.commit()
    
    def learn_struct_pattern(self, symbol_name: str, likely_type: str = None,
                            field_map: Dict = None, confidence: float = 0.5):
        """
        Learn or update a struct pattern from a successful decompilation.
        
        Args:
            symbol_name: Symbol like 'D_800F83D0'
            likely_type: Type name like 'Unk800F83D0' or 'BattleStruct'
            field_map: Dict of {offset: (field_name, field_type)}
            confidence: 0.0-1.0 confidence score
        """
        import json
        cursor = self.conn.cursor()
        
        field_map_json = json.dumps(field_map) if field_map else None
        
        # Check if we already have this symbol
        cursor.execute("""
            SELECT seen_count, confidence FROM struct_patterns 
            WHERE symbol_name = ?
        """, (symbol_name,))
        existing = cursor.fetchone()
        
        if existing:
            # Update existing pattern (increase confidence with more sightings)
            new_seen = existing['seen_count'] + 1
            # Weighted average of confidence
            new_confidence = (existing['confidence'] * existing['seen_count'] + confidence) / new_seen
            
            cursor.execute("""
                UPDATE struct_patterns 
                SET likely_type = COALESCE(?, likely_type),
                    field_map = COALESCE(?, field_map),
                    confidence = ?,
                    seen_count = ?,
                    updated_at = CURRENT_TIMESTAMP
                WHERE symbol_name = ?
            """, (likely_type, field_map_json, new_confidence, new_seen, symbol_name))
        else:
            # Insert new pattern
            cursor.execute("""
                INSERT INTO struct_patterns 
                (symbol_name, likely_type, field_map, confidence)
                VALUES (?, ?, ?, ?)
            """, (symbol_name, likely_type, field_map_json, confidence))
        
        self.conn.commit()
    
    def get_known_type(self, symbol_name: str, min_confidence: float = 0.3) -> Optional[Dict]:
        """
        Get known type information for a symbol.
        
        Returns:
            Dict with 'likely_type', 'field_map', 'confidence' if known, else None
        """
        import json
        cursor = self.conn.cursor()
        cursor.execute("""
            SELECT likely_type, field_map, confidence, seen_count 
            FROM struct_patterns 
            WHERE symbol_name = ? AND confidence >= ?
        """, (symbol_name, min_confidence))
        
        row = cursor.fetchone()
        if not row:
            return None
        
        return {
            'likely_type': row['likely_type'],
            'field_map': json.loads(row['field_map']) if row['field_map'] else {},
            'confidence': row['confidence'],
            'seen_count': row['seen_count']
        }
    
    def get_all_known_types(self, min_confidence: float = 0.3) -> Dict[str, Dict]:
        """Get all known struct patterns above confidence threshold."""
        import json
        cursor = self.conn.cursor()
        cursor.execute("""
            SELECT symbol_name, likely_type, field_map, confidence, seen_count 
            FROM struct_patterns 
            WHERE confidence >= ?
            ORDER BY confidence DESC, seen_count DESC
        """, (min_confidence,))
        
        return {
            row['symbol_name']: {
                'likely_type': row['likely_type'],
                'field_map': json.loads(row['field_map']) if row['field_map'] else {},
                'confidence': row['confidence'],
                'seen_count': row['seen_count']
            }
            for row in cursor.fetchall()
        }
    
    def get_functions_using_symbol(self, symbol_name: str) -> List[str]:
        """Get list of functions that reference a symbol."""
        cursor = self.conn.cursor()
        cursor.execute("""
            SELECT DISTINCT function_name 
            FROM symbol_usage 
            WHERE symbol_name = ?
        """, (symbol_name,))
        return [row['function_name'] for row in cursor.fetchall()]
    
    def mark_struct_success(self, symbol_name: str):
        """Increment successful use counter for a struct pattern."""
        cursor = self.conn.cursor()
        cursor.execute("""
            UPDATE struct_patterns 
            SET successful_uses = successful_uses + 1,
                confidence = MIN(1.0, confidence + 0.05)
            WHERE symbol_name = ?
        """, (symbol_name,))
        self.conn.commit()
    
    def get_struct_learning_stats(self) -> Dict:
        """Get statistics on struct learning."""
        cursor = self.conn.cursor()
        
        cursor.execute("SELECT COUNT(*) as total FROM struct_patterns")
        total = cursor.fetchone()['total']
        
        cursor.execute("""
            SELECT COUNT(*) as high_conf 
            FROM struct_patterns 
            WHERE confidence >= 0.7
        """)
        high_conf = cursor.fetchone()['high_conf']
        
        cursor.execute("""
            SELECT COUNT(*) as medium_conf 
            FROM struct_patterns 
            WHERE confidence >= 0.3 AND confidence < 0.7
        """)
        medium_conf = cursor.fetchone()['medium_conf']
        
        return {
            'total_patterns': total,
            'high_confidence': high_conf,
            'medium_confidence': medium_conf,
            'low_confidence': total - high_conf - medium_conf
        }
