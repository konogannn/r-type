# Security Review (C7)

## Objective
Address requirement C7 by regularly reviewing existing protocols and components in light of new security vulnerabilities, and by driving adoption of high technological standards within the project. This document provides a structured security study of the benchmarked technologies used in the R-Type engine and game.

## Scope (Technologies Benchmarked)
- Networking & Protocol: UDP-based custom protocol with lightweight reliability (ACKs, sequence numbers).
- Networking Library: Boost.Asio.
- Client Rendering/Input/Audio: SFML (alternatives considered: SDL, Raylib).
- Concurrency: C++ `std::thread`, atomics, condition variables.
- Serialization: Compact binary structs, delta snapshots.
- Build & Dependencies: CMake, vcpkg (pinned versions), toolchain flags.

## Threat Model & Attack Surface
- Network layer: packet spoofing, replay attacks, packet storms (DoS), amplification abuse, malformed payloads.
- Protocol parsing: buffer overflows, integer overflows/underflows, insecure deserialization, schema confusion.
- Concurrency: race conditions, deadlocks, TOCTOU issues, use-after-free.
- Resource management: memory leaks, unbounded queues/buffers, file descriptor exhaustion.
- Client rendering/input: crafted assets causing crashes; input injection; focus hijacking.
- Supply chain: dependency vulnerabilities, compromised releases, misconfigurations.

## Recent Vulnerabilities (study guidance)
This section should be maintained with current findings. Populate with entries during each review cycle.  
- Boost ecosystem: monitor Asio and Boost releases for fixes or CVEs.  
- SFML/SDL: check project release notes, issue trackers, and distribution advisories.  
- Toolchains: compiler/linker security flags updates; libc issues.  
- OS/Kernel: Linux distribution security advisories relevant to networking.  

References to consult:
- NVD (nvd.nist.gov), CVE Details, CERT-FR, CISA KEV, project release notes/changelogs.

## Mitigations & Standards (Chosen Options)
- Transport & Protocol:
  - Use UDP for gameplay + reliability layer (ACKs, sequence numbers, replay window).
  - Validate all payloads: size bounds, type ranges, enum checks, checksum/HMAC for critical control messages.
  - Rate-limiting per client IP/port and per message type; drop malformed/oversized packets early.
- Serialization:
  - Fixed-size fields with explicit endianness; avoid implicit padding; bounds-checked reads/writes.
  - Delta snapshots for world updates; optional compression (RLE/delta) after profiling.
- Concurrency:
  - Single authoritative game loop thread; short non-blocking I/O handlers; guarded or lock-free queues.
  - Avoid per-client threads; use thread pool with measured contention.
- Build & Runtime Hardening:
  - Treat warnings as errors; enable sanitizers in debug/CI: Address, UndefinedBehavior (ASan/UBSan).
  - Static analysis: clang-tidy (core checks), cppcheck.
- Dependency Management:
  - Pin versions in vcpkg; track upstream advisories; schedule upgrades.
  - Verify checksums/signatures where available.
- Logging & Monitoring:
  - Structured logs for rejected packets, rate limit events, parse errors.
  - Basic metrics: per-client message rates, drop counts, queue sizes.

## Why These Standards (Rationale)
- UDP + targeted reliability avoids TCP head-of-line blocking, preserving responsiveness under loss.
- Strict validation and versioned schemas reduce crash/compromise risk from malformed inputs.
- Hardening flags and sanitizers catch classes of memory bugs early, lowering long-term technical debt.
- Pinned dependencies and scheduled upgrades minimize exposure to known CVEs.
- Rate limiting and metrics provide practical controls against DoS without excessive complexity.

## Action Plan & Checklist
- Weekly: triage advisories; update findings; file issues for mitigations.
- Pre-release: dependency updates; security regression tests.
- Incident response: steps to roll back, hotfix, and communicate.

## Appendices
- Test ideas: fuzz input parsers (libFuzzer), property-based tests for serialization, simulate packet loss/reorder.
- Protocol versioning policy: backward-compatible changes preferred; deprecations with grace period.
