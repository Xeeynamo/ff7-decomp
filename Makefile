OVL_US += BATTLE/BATINI.X
OVL_US += BATTLE/BATRES.X
OVL_US += BATTLE/BATTLE.X
OVL_US += BATTLE/BROM.X
OVL_US += FIELD/FIELD.BIN
OVL_US += MINI/CHOCOBO.BIN
OVL_US += WORLD/WORLD.BIN

.PHONY: all
all: disks build

.PHONY: build
build: bin/cc1-psx-26 bin/cc1-psx-272 bin/str disks/us/FIELD/FIELD.BIN.dec
	@go run ./tools/builder build config/us.yaml
	@ninja
	@mkdir -p expected/build
	@rm -rf expected/build
	@cp -r build expected/

disks/us/FIELD/FIELD.BIN.dec:
	@for f in $(OVL_US); do \
		tail --bytes=+9 "disks/us/$$f" | gzip -cd > "disks/us/$$f.dec"; \
	done

.PHONY: disks
disks: disks/us

disks/%.iso:
	bchunk "disks/$*.bin" "disks/$*.cue" "$@"
	mv "disks/$*.iso01.iso" "$@"
disks/us: disks/Final\ Fantasy\ VII\ (USA)\ (Disc\ 1).iso
	7z x "$<" -o$@
disks/betaus: disks/Final\ Fantasy\ VII\ (USA)\ (Interactive\ Sampler\ CD).iso
	7z x "$<" -o$@

.PHONY: clean
clean:
	@git clean -xdf . --exclude=disks/ --exclude=.venv/ --exclude=bin/
	@git clean -Xfd asm/
	@git clean -Xfd build/
	@git clean -Xfd config/

.PHONY: format
format: bin/clang-format
	find src/ include/ -type f \( -name '*.c' -o -name '*.h' \) \
		! -path 'include/psxsdk/*' -print0 | \
		xargs -0 -P$$(nproc) bin/clang-format -i

.PHONY: report
report: build
	@go run ./tools/builder report config/us.yaml build/report.json

.PHONY: requirements
requirements:
	python3 -m venv .venv
	.venv/bin/pip3 install -r requirements.txt

build/us/%.o: %
	ninja $@

bin/str: tools/str.c
	gcc -s -o $@ -O2 $<
	chmod +x $@

bin/%: bin/%.gz
	sha256sum --check $<.sha256
	gzip -kcd $< > $@
	touch $@
	chmod +x $@
bin/%.gz: bin/%.gz.sha256
	wget -O $@ https://github.com/Xeeynamo/ff7-decomp/releases/download/init/$*.gz
