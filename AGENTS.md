# Road Rash: Jailbreak

Applies to this project. `[XPORT_ROOT]` denotes the shared xport repository root resolved from `xport-project.json`; the tag is documentation notation, not a literal path. Read [shared STARTUP.md](../xport/STARTUP.md) and [shared PIPELINE.md](../xport/tools/PIPELINE.md) before work. STARTUP owns project phase gates; PIPELINE owns PSX/MIPS methodology, SQL contracts, TODO/WIP/DONE gates, shared formatting and tool commands. Improve reusable behavior in `../xport/tools` and update its `PIPELINE.md`; keep Road Rash facts here.

## Project facts

- Full project name is `Road Rash: Jailbreak`; the exact native short name is `RRJ`.
- The user chose C. Game sources are `.c`/`.h` under `src`. Build `src/platform/win/RRJ.sln` with VS2022 Debug or Release x86; the solution maps x86 to Win32. Output is `bin/RRJ.exe`, working directory is `bin`, and intermediates belong in `_build`.
- Compile the canonical `[XPORT_ROOT]/src` runtime directly. Shared `DRAM`, `SCRATCHPAD`, `VRAM`, SPU RAM, GPU rasterization, PAD input, audio output and Win32 `WinMain` are the only maintained implementations; `src/platform/win` contains only build-system files.
- The reviewed CUE has one MODE2/2352 data track and no Red Book AUDIO tracks. Keep `capabilities.redbook_audio` disabled; do not pass XA or other data-track music through `convert_music`.
- NFS4 aliases remain hypotheses unless Road Rash MIPS and runtime evidence independently prove the relationship.
- Agreed dummy/WIP scope is memory card access, physical CD reads, STR playback and MDEC decode. Do not generalize these boundaries to game logic, sound, input, GPU, menu or race behavior.
- Original images are `SLUS_010.53`, `RASHCDF.BIN`, `RASHCDG.BIN` and `RASHCDI.BIN`; default query image is `SLUS_010.53`. IDA exports are immutable inputs under `orig/images`, project IDA layouts are under `tools/ida`, the preserved Ghidra source is `orig/ghidra/classification.json`, and the reviewed active classification is `status/ghidra/classification.json`.
- Active analysis database is `status/analysis.sqlite`; `orig/analysis.sqlite` is the preserved historical database. Status authority is `status/translation-ledger.json` plus the active database. The migration retained 335 WIP entries and did not promote any function to DONE.
- Known fail-closed source mappings are `RASHCDG.BIN:800B6B04` and `SLUS_010.53:8003BFE8`; both ledger entries point to combined `src/race_pause.c` without a unique address-bearing C identity. Keep them unmapped until the ledger records reviewed implementation identities.
- Preserve the existing 335 WIP implementations. The remaining 1473 TODO functions are an inventory, not a pre-record blocker: establish the trace-ready frontier first, then apply the shared derived-branch rule whenever convergence reaches a causal TODO.
- Commands from the project root use `python -B ../xport/tools/xport.py --project . TOOL ...`. Do not execute implementations from `tools/archive`.
- Brief comments start with a capital letter and omit the final period.

## Emulator and pipeline state

- The shared executable is `[XPORT_ROOT]/tools/duckstation/distribution/duckstation-qt-x64-ReleaseLTCG.exe`; project directories contain data/settings only.
- Reserved loopback ports are audit 2401, trace 2402 and user 2403. Never reuse them in another project. Role data directories are `tools/duckstation/data/{audit,trace,user}`.
- Historical BIOS, settings and save states were copied into the isolated role data directories. The old standalone emulator, GDB, probes and local workflow scripts are retained only in `tools/archive/legacy-pipeline-20260923` with a SHA-256 manifest.
- The audited Road Rash trace profile is `tools/duckstation/xport-trace-profile.ini`; its evidence and declared comparison scope are recorded in `status/toolset-validation/trace-contract.md`. Never reuse Fighting Force addresses, layouts, exclusions or adapter semantics.
- The required headless record/stop/decode gate passed for the current emulator and profile identities in `status/toolset-validation/trace-capture-smoke.json`; repeat it after any trace profile, recorder or emulator patch change before asking for another manual recording.
- The native continuation contract is `RoadRash race-loop checkpoint ABI 1`, converted by `tools/rrj_stage_adapter.py`. It resumes at `SLUS_010.53:80012370`; until the containing `80012224` race-loop branch is translated, native replay must stop through the WIP journal and produce `NEEDS_CODE` rather than silently simulate it.
- The next new `record → converge` cycle is the authorized proving run for the shared TODO-derived-branch workflow. Record its branch selection, translations, replay result and any ambiguity in `status/toolset-validation/todo-derived-branch-acceptance.json`.

## Preserved evidence

- Historical probe/oracle results under `status` remain evidence, not maintained commands. Their old launchers and harnesses are archived and must not be restored into active dispatch.
- The legacy translation source is `status/decompilation/translations.json`; its machine-readable successor is `status/translation-ledger.json`.
- Existing function tests and narrow replays support their recorded scopes only. They do not establish full gameplay equivalence or DONE status under the shared pipeline.
