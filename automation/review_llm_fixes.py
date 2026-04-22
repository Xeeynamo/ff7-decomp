#!/usr/bin/env python3
"""
Review LLM Auto-Fixes

Helper tool to review functions that were automatically fixed by the LLM.
Shows functions marked as 'decompiled_needs_refine' from LLM fixes.

Usage:
    python review_llm_fixes.py --list          # List all LLM-fixed functions
    python review_llm_fixes.py --log           # Show audit log
    python review_llm_fixes.py --promote func  # Promote to 'decompiled' after manual review
    python review_llm_fixes.py --stats         # Statistics on LLM fixes
"""

import argparse
import sqlite3
import json
from pathlib import Path
from datetime import datetime

PROJECT_ROOT = Path(__file__).parent.parent
DB_PATH = Path(__file__).parent / "functions.db"
AUDIT_LOG = Path(__file__).parent / "llm_fixes_audit.log"


def list_llm_fixes():
    """List all functions with LLM auto-fixes."""
    conn = sqlite3.connect(DB_PATH)
    conn.row_factory = sqlite3.Row
    cursor = conn.cursor()
    
    # Find functions with LLM fix notes
    cursor.execute("""
        SELECT name, status, c_file_path, notes, updated_at
        FROM functions
        WHERE notes LIKE '%LLM auto-fix%'
        ORDER BY updated_at DESC
    """)
    
    results = cursor.fetchall()
    
    if not results:
        print("No LLM-fixed functions found.")
        return
    
    print(f"\n{'='*80}")
    print(f"LLM AUTO-FIXED FUNCTIONS ({len(results)} total)")
    print(f"{'='*80}\n")
    
    needs_refine = []
    decompiled = []
    
    for row in results:
        # Try to parse JSON notes
        try:
            notes_data = json.loads(row['notes'])
            if 'llm_analysis' in notes_data:
                # This is just analysis, not a fix
                continue
        except (json.JSONDecodeError, TypeError):
            pass
        
        # Check if it's an actual fix
        if 'LLM auto-fix' not in row['notes']:
            continue
        
        # Extract confidence
        confidence = 'unknown'
        if '(high confidence)' in row['notes']:
            confidence = 'high'
        elif '(medium confidence)' in row['notes']:
            confidence = 'medium'
        elif '(low confidence)' in row['notes']:
            confidence = 'low'
        
        entry = {
            'name': row['name'],
            'status': row['status'],
            'file': row['c_file_path'],
            'confidence': confidence,
            'updated': row['updated_at'],
            'notes': row['notes']
        }
        
        if row['status'] == 'decompiled_needs_refine':
            needs_refine.append(entry)
        else:
            decompiled.append(entry)
    
    # Show functions needing review
    if needs_refine:
        print(f"⚠️  NEEDS MANUAL REVIEW ({len(needs_refine)} functions):")
        print(f"{'-'*80}")
        for entry in needs_refine:
            print(f"  {entry['name']}")
            print(f"    File: {entry['file']}")
            print(f"    Confidence: {entry['confidence']}")
            print(f"    Updated: {entry['updated']}")
            explanation = entry['notes'].split('): ', 1)[-1] if '): ' in entry['notes'] else ''
            if explanation:
                print(f"    Fix: {explanation[:100]}...")
            print()
    
    # Show already-decompiled (high confidence)
    if decompiled:
        print(f"\n✅ HIGH CONFIDENCE (already marked 'decompiled' - {len(decompiled)} functions):")
        print(f"{'-'*80}")
        for entry in decompiled[:5]:  # Show first 5
            print(f"  {entry['name']} ({entry['file']})")
        if len(decompiled) > 5:
            print(f"  ... and {len(decompiled) - 5} more")
    
    conn.close()


def show_audit_log():
    """Display the audit log."""
    if not AUDIT_LOG.exists():
        print("No audit log found.")
        return
    
    print(f"\n{'='*80}")
    print("LLM FIX AUDIT LOG")
    print(f"{'='*80}\n")
    
    with open(AUDIT_LOG, 'r') as f:
        lines = f.readlines()
    
    if not lines:
        print("Audit log is empty.")
        return
    
    print(f"{'Timestamp':<20} {'Function':<25} {'Confidence':<10} {'Error Type'}")
    print(f"{'-'*80}")
    
    for line in lines[-20:]:  # Show last 20
        parts = line.strip().split('\t')
        if len(parts) >= 6:
            timestamp = parts[0].split('T')[0]  # Just date
            func_name = parts[1]
            confidence = parts[3]
            error_type = parts[4][:30] if len(parts[4]) > 30 else parts[4]
            print(f"{timestamp:<20} {func_name:<25} {confidence:<10} {error_type}")
    
    if len(lines) > 20:
        print(f"\n... showing last 20 of {len(lines)} entries")


def promote_function(func_name):
    """Promote a function from needs_refine to decompiled after manual review."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    # Check current status
    cursor.execute("SELECT status, notes FROM functions WHERE name = ?", (func_name,))
    row = cursor.fetchone()
    
    if not row:
        print(f"Function {func_name} not found in database.")
        return
    
    status, notes = row
    
    if status != 'decompiled_needs_refine':
        print(f"Function {func_name} has status '{status}', not 'decompiled_needs_refine'.")
        print("Only functions flagged for review can be promoted.")
        return
    
    # Update status
    cursor.execute("""
        UPDATE functions 
        SET status = 'decompiled',
            notes = ?,
            updated_at = CURRENT_TIMESTAMP
        WHERE name = ?
    """, (f"{notes} [Manually reviewed and promoted]", func_name))
    
    conn.commit()
    conn.close()
    
    print(f"✅ Promoted {func_name} to 'decompiled' status.")
    print(f"   Function has been manually reviewed and approved.")


def show_statistics():
    """Show statistics on LLM fixes."""
    conn = sqlite3.connect(DB_PATH)
    cursor = conn.cursor()
    
    # Count LLM fixes by status
    cursor.execute("""
        SELECT status, COUNT(*) as count
        FROM functions
        WHERE notes LIKE '%LLM auto-fix%'
        GROUP BY status
    """)
    
    stats = dict(cursor.fetchall())
    
    # Count by confidence (from audit log)
    confidence_counts = {'high': 0, 'medium': 0, 'low': 0}
    if AUDIT_LOG.exists():
        with open(AUDIT_LOG, 'r') as f:
            for line in f:
                parts = line.strip().split('\t')
                if len(parts) >= 4:
                    conf = parts[3]
                    if conf in confidence_counts:
                        confidence_counts[conf] += 1
    
    print(f"\n{'='*60}")
    print("LLM FIX STATISTICS")
    print(f"{'='*60}\n")
    
    print("By Status:")
    total = sum(stats.values())
    for status, count in stats.items():
        print(f"  {status}: {count} ({count/total*100:.1f}%)")
    
    print(f"\nBy Confidence:")
    conf_total = sum(confidence_counts.values())
    if conf_total > 0:
        for conf, count in confidence_counts.items():
            print(f"  {conf}: {count} ({count/conf_total*100:.1f}%)")
    
    print(f"\nTotal LLM fixes attempted: {conf_total}")
    print(f"Functions needing review: {stats.get('decompiled_needs_refine', 0)}")
    
    conn.close()


def main():
    parser = argparse.ArgumentParser(description='Review LLM auto-fixes')
    parser.add_argument('--list', action='store_true', help='List all LLM-fixed functions')
    parser.add_argument('--log', action='store_true', help='Show audit log')
    parser.add_argument('--stats', action='store_true', help='Show statistics')
    parser.add_argument('--promote', metavar='FUNC', help='Promote function to decompiled after review')
    
    args = parser.parse_args()
    
    if args.list:
        list_llm_fixes()
    elif args.log:
        show_audit_log()
    elif args.stats:
        show_statistics()
    elif args.promote:
        promote_function(args.promote)
    else:
        parser.print_help()


if __name__ == '__main__':
    main()
