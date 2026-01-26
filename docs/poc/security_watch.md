# Security Watch Process (C7)

## Objective
Meet C7 by demonstrating continuous security monitoring (veille) relevant to the project: regularly revising protocols/components in light of new vulnerabilities and adopting modern security practices/tools.

## Sources to Monitor
- National/sector advisories: CERT-FR alerts and bulletins.
- Project ecosystems:
  - Boost / Boost.Asio release notes and issue trackers.
  - SFML/SDL release notes, advisories, distro packages.
  - vcpkg updates and security notices.
- OS & Toolchains: Linux distribution security advisories (Ubuntu), Clang toolchain notes.
- Community: security blogs.

## Cadence & Workflow
- **Weekly triage**:
  - We checke security vulnerabilities on a weekly basis.

## Roles & Ownership
- Security Champion: coordinates triage/reviews, ensures actions are tracked.
- Owners per subsystem: networking, client rendering, build/deps.

## Decision Policy
- Severity mapping: Critical/High -> immediate mitigation or temporary disable; Medium -> schedule fix; Low -> monitor.
- Protocol revisions: changes require version bump and compatibility notes.
- Dependency updates: pin, test, deploy; rollback plan prepared.

## Documentation & Evidence
- Keep the log under version control (this file + findings in `security_review.md`).
- Reference issues/PRs; attach test results/screenshots for mitigations.

## Tooling Recommendations
- CI steps: build; run unit tests.
- Static analysis: clang-tidy; cppcheck.
- Packet tests: scripted packet loss/reorder/dup scenarios.

