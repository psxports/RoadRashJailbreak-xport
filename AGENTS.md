# Road Rash: Jailbreak

Resolve `[XPORT_ROOT]` from `xport-project.json` and read `[XPORT_ROOT]/AGENTS.md`. Read the routed startup document only for phase-gated preparation.

## Identity and build

- Native short name `RRJ`; C/VS2022 x86: `src/platform/win/RRJ.sln` -> `bin/RRJ.exe`; working directory `bin`; intermediates `_build`
- Original images: `SLUS_010.53`, `RASHCDF.BIN`, `RASHCDG.BIN`, `RASHCDI.BIN`; default query image: `SLUS_010.53`
- Immutable IDA exports: `orig/images`; profiles: `tools/ida`; reviewed Ghidra classification: `status/ghidra/classification.json`
- Authorities: `status/analysis.sqlite` and `status/translation-ledger.json`; preserved predecessor: `status/decompilation/translations.json`
- Commands: `python -B ../xport/tools/xport.py --project . TOOL ...`

## Project constraints

- Compile the shared `[XPORT_ROOT]/src` runtime directly; project-local runtime copies are forbidden
- The CUE has one `MODE2/2352` data track and no Red Book tracks. Keep `capabilities.redbook_audio` disabled; XA/data-track audio is not `convert_music` input
- Agreed WIP scope: memory card, physical CD reads, STR playback and MDEC decode. Game logic, sound, input, GPU, menu and race behavior remain real
- Web packaging omits raw `ALBUM*.ALB`, `STREAM*.STR` and `ZZZDUMMY.FIL` disc assets covered by that WIP scope
- NFS4 aliases are hypotheses until Road Rash MIPS and runtime evidence prove them
- Keep `RASHCDG.BIN:800B6B04` and `SLUS_010.53:8003BFE8` unmapped until each combined region has a unique reviewed C identity
- Preserve existing WIP implementations. Global TODO zero is not a record blocker; converge translates the complete causal dependency branch on demand

## Trace and evidence

- Reserved GDB ports: audit `2401`, trace `2402`, user `2403`; profile: `tools/duckstation/xport-trace-profile.ini`
- Trace contract: `status/toolset-validation/trace-contract.md`; adapter: `tools/rrj_stage_adapter.py`; native continuation: `RoadRash race-loop checkpoint ABI 1` at `SLUS_010.53:80012370`
- Until the containing `80012224` race-loop branch is translated, replay must return `NEEDS_CODE`, never simulate it
- Historical probes and `tools/archive/legacy-pipeline-20260923` are evidence only, not maintained commands
- Audit/replay binding contract is `xport-audit-replay-v1`; RRJ adapters keep race-specific ABI/device facts while shared xport code owns checkpoint publication, probe lifecycle, frame comparison and WIP JSONL encoding
- Before C/H work use `X query 0xADDRESS --image IMAGE --audit-context`; after changes use `X code_refresh`
