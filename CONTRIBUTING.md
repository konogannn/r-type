# 🧭 How to Contribute to **R-Type**

Welcome! 👋 
Thank you for your interest in contributing to **R-Type** — [insert a short description of the project]  
This document explains the rules and best practices for contributing to this repository.

---

## 🏗️ Project Structure

The **R-Type** project is divided into 2 main parts:

| Component  | Path      |
| ---------- | --------- |
| **Server** | `/server` |
| **Client** | `/client` |

---

## 🧩 General Contribution Workflow

1. **Fork the repository** (if you are not a member of the Redemption team).

    * Go to [konogannn/r-type](https://github.com/konogannn/r-type/).
    * Click **Fork** to create your own copy.
    * Clone your fork locally:

    ```bash
    $> git clone https://github.com/<your-username>/r-type.git
    $> cd r-type
    ```
    * Add the original repo as an upstream:

    ```bash
    $> git remote add upstream https://github.com/konogannn/r-type.git
    ```

2. **Create a feature branch**:

    ```bash
    $> git checkout -b feat/your-feature-name
    ```

3. **Write clear and conventional commits** (in **English**).

    ⚠️ **COMMIT CONVENTION:** We use the format `[TYPE](scope): commit body`.

    * **TYPE** should be one of: `FEAT`, `FIX`, `DOCS`, `REFACTOR`, `TEST`, `CHORE`.
    * **scope** (optional) specifies the affected part (e.g., `client`, `server`).
    * **Examples:**

      * `[FEAT](client): add new sprites for the Bydos`
      * `[FIX](server): correct entities movements`
      * `[DOCS]: update setup instructions`

4. **Push your branch** and open a **Pull Request (PR)**.

    * **Title** must follow the same **conventional format** (e.g., `[FEAT]: A clear description`).
    * **Description** is **mandatory**. You may use GitHub Copilot to write it, but review it before submitting.

---

## 👥 Pull Request Review Rules

* **Reviewers:**
    You must add **at least one other contributor** as a reviewer.
    Copilot can be added as a bonus reviewer, **but not as the only one**.

* **Checks:**
    All automated checks (lint, tests, CI) **must pass** before the PR can be merged.

* **Documentation:**
    Ensure that any changes to the web client or mobile application are reflected in the documentation, especially if they affect user experience or functionality.

---

## ✅ Code Quality Guidelines

* Follow the existing code style and structure.
* Keep commits small and meaningful.
* Update documentation or comments when behavior changes.
* Test your code locally before submitting a PR.

---

## 🪄 Forking Workflow (External Contributors)

If you are **not part of the Redemption team**, please work from your fork:

1. Fork the repo and clone it locally.
2. Create a branch for your changes.
3. Push your branch to your fork.
4. Open a PR **from your fork to `main`** in [konogannn/r-type](https://github.com/konogannn/r-type/).

This ensures maintainers can review and merge your work safely.

---

## 🧹 Before You Merge

* ✅ All tests pass.
* ✅ Linter passes.
* ✅ At least one reviewer has approved.
* ✅ PR title and commits are **English and follow the `[TYPE](scope): body` convention**.
* ✅ The PR description clearly explains the change.

---

## 💬 Need Help?

If you have any questions, please contact us by email or tag one of the maintainers in your pull request.

Thanks for contributing to **R-Type**! 👾
