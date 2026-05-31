{
  description = "Final Fantasy VII (PS1) Decompilation";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachSystem [ "x86_64-linux" ] (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};

        # MIPS little-endian cross toolchain for PS1.  The derivation
        # below wraps the actual tools (named
        # mipsel-unknown-linux-gnu-{as,ld,...} on many nixpkgs
        # versions) so they are also reachable as
        # mipsel-linux-gnu-{as,ld,...}, which the Ninja build rules
        # expect.
        mipsPkgs = pkgs.pkgsCross.mipsel-linux-gnu.buildPackages;

        mips-tools = pkgs.runCommand "mipsel-linux-gnu-tools" {
          buildInputs = [ mipsPkgs.binutils mipsPkgs.gcc ];
        } ''
          mkdir -p $out/bin

          # For every tool prefixed with the actual target triple,
          # create a symlink that uses the shorter prefix the project
          # expects.
          #
          #   mipsel-unknown-linux-gnu-as -> mipsel-linux-gnu-as
          #   mipsel-unknown-linux-gnu-ld -> mipsel-linux-gnu-ld
          #   ...
          for dir in ${mipsPkgs.binutils}/bin ${mipsPkgs.gcc}/bin; do
            for f in "$dir"/*; do
              base=$(basename "$f")
              alias_name=$(echo "$base" | sed 's/^mipsel-unknown-linux-gnu-/mipsel-linux-gnu-/')
              if [ -f "$f" ] || [ -L "$f" ] && [ ! -e "$out/bin/$alias_name" ]; then
                ln -s "$f" "$out/bin/$alias_name"
              fi
            done
          done
        '';

        python-with-pkgs = pkgs.python3.withPackages (ps: with ps; [
          pyyaml
          colorama
          watchdog
          pycparser
        ]);

      in {
        devShells.default = pkgs.mkShell {
          name = "ff7-decomp";

          packages = with pkgs; [
            python-with-pkgs
            go
            ninja
            p7zip
            bchunk
            wget
            gzip
            glibc
            stdenv.cc.cc.lib
            mips-tools
          ];

          shellHook = ''
            # Verify cross-toolchain
            for tool in as ld objcopy cpp; do
              if ! command -v "mipsel-linux-gnu-$tool" &>/dev/null; then
                echo "WARNING: mipsel-linux-gnu-$tool not found in PATH"
              fi
            done

            # Python venv as done with `make requirements`
            if [ ! -d .venv ]; then
              ${pkgs.python3}/bin/python3 -m venv .venv
            fi
            source .venv/bin/activate
            if [ -f requirements.txt ]; then
              pip install -q -r requirements.txt 2>/dev/null \
                || echo "pip install failed; run 'pip install -r requirements.txt' manually"
            fi

            # Make libstdc++.so.6 available for Python C++ extensions
            # (e.g. the levenshtein package used by asm-differ)
            export LD_LIBRARY_PATH="${pkgs.stdenv.cc.cc.lib}/lib:$LD_LIBRARY_PATH"

            cat <<'EOF'

            FF7 Decomp — Nix dev shell ready

            Place PS1 game images in disks/:
              disks/
              ├── Final Fantasy VII (USA) (Disc 1).bin
              ├── Final Fantasy VII (USA) (Disc 1).cue
              ├── Final Fantasy VII (USA) (Interactive Sampler CD).bin   (optional)
              └── Final Fantasy VII (USA) (Interactive Sampler CD).cue   (optional)

            First run: `make disks` for the first time
            Then run: `make build`
            EOF
          '';
        };
      });
}
