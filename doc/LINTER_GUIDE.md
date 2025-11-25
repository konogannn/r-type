# ClangFormat - Linter Guide

## 📋 Overview

This project uses **ClangFormat** with **Google C++ Style Guide** to maintain consistent code formatting across all `.cpp` and `.hpp` files.

## 🚀 Installation

### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install clang-format
```

### macOS
```bash
brew install clang-format
```

### Verify Installation
```bash
clang-format --version
```

## 🛠️ Usage

### Auto-Format All Files
Automatically formats all `.cpp` and `.hpp` files according to Google style:
```bash
make format
```

**⚠️ Important:** This command modifies files in place. It's recommended to commit your changes before running this command!

### Quick Workflow
```bash
# After making code changes
cd build
make format        # Auto-format all files
cd ..
git add .
git commit -m "your message"
git push
```

## 📐 Google Style Configuration

The `.clang-format` file at the root of the project defines the following rules:

- **Indentation:** 2 spaces (no tabs)
- **Line width:** 80 characters
- **Braces:** Attached style (K&R)
- **Pointers/References:** Left-aligned (`int* ptr`, not `int *ptr`)
- **Namespaces:** No indentation inside namespaces
- **Includes:** Sorted and grouped

## 🔄 CI/CD Integration

### GitHub Actions
The workflow `.github/workflows/clang-format-check.yml` automatically checks formatting on:
- Every push to any branch
- Every pull request to `main` or `develop`

If formatting issues are detected, the CI will **fail** and display the errors.

**To fix before pushing:**
```bash
make format-fix
git add .
git commit -m "fix: apply clang-format"
git push
```

## 📝 Best Practices

### Before Committing
Always run format before committing:
```bash
cd build
make format
cd ..
git add .
git commit -m "your commit message"
```

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

#### CLion/IntelliJ
1. Go to `Settings > Editor > Code Style > C/C++`
2. Set "Scheme" to "Google"
3. Enable "Reformat code on save"

## 🐛 Troubleshooting

### "clang-format: command not found"
Install clang-format (see Installation section above)

### "No such file or directory"
Make sure you're in the project root directory and have run CMake:
```bash
cd /path/to/r-type
mkdir -p build && cd build
cmake ..
```

### Formatting conflicts with existing code
Run format to align all existing code:
```bash
cd build
make format
```

## 📚 Additional Resources

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [ClangFormat Documentation](https://clang.llvm.org/docs/ClangFormat.html)
- [ClangFormat Style Options](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)

## ⚡ Quick Reference

| Command | Description |
|---------|-------------|
| `make format` | Auto-format all C++ files |

---

**Note:** The linter runs automatically in CI/CD but NOT during regular builds to avoid slowing down compilation.
