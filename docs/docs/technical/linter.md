---
id: linter
title: Linter
description: A clang-format tutorial
sidebar_position: 4
---

# Linter

## Overview

This project uses **ClangFormat** with a fork of **Google C++ Style Guide** to maintain consistent code formatting across all `.cpp`, `.hpp` and `.tpp` files.

## Ensure clang-format is installed
```bash
clang-format --version
```

## 🛠️ Usage

### Auto-Format All Files
Automatically formats all `.cpp`, `.hpp` and `.tpp` files according to Google style:
```bash
make format
```

:::danger Important
This command modifies files in place. It's recommended to commit your changes before running this command!
:::

## 📐 Google Style Configuration

The `.clang-format` file at the root of the project defines our presonal rules overloading Google ones

## 🔄 CI/CD Integration

### GitHub Actions
The workflow `.github/workflows/lint.yml` automatically checks formatting on:
- Every pull request where relevant files have been modified

If formatting issues are detected, the CI will **fail** and display the errors.

## 📝 Best Practices
### During Development
You can configure your IDE to auto-format on save:

#### VS Code
1. Install the "C/C++" extension
2. Add to `.vscode/settings.json`:
```json
{
  "editor.formatOnSave": true,
  "C_Cpp.clang_format_style": "file"
}
```

#### CLion
1. Go to `Settings > Editor > Code Style > C/C++`
2. Set "Scheme" to "file"
3. Enable "Reformat code on save"

## 📚 Additional Resources

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [ClangFormat Documentation](https://clang.llvm.org/docs/ClangFormat.html)
- [ClangFormat Style Options](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)
