# GitHub Actions CI/CD Guide

## What is CI/CD?

**CI/CD** stands for **Continuous Integration** and **Continuous Deployment/Delivery**:

- **Continuous Integration (CI)**: Automatically builds and tests your code whenever you push changes
- **Continuous Deployment (CD)**: Automatically deploys your code (optional, not used here)

In this project, CI automatically:
- ✅ Builds your code on Linux and macOS
- ✅ Runs all unit tests
- ✅ Reports if anything breaks

## How GitHub Actions Works

GitHub Actions runs workflows defined in `.github/workflows/ci.yml`. When you push code, GitHub automatically:
1. Spins up a virtual machine (Ubuntu or macOS)
2. Checks out your code
3. Installs dependencies
4. Builds your project
5. Runs tests
6. Reports results

## Step-by-Step: Using CI/CD

### Step 1: Make Sure Your Code is Committed

First, check what files have changed:

```bash
git status
```

You should see your modified files. Add them to staging:

```bash
# Add all changes
git add .

# Or add specific files
git add src/tui.cpp include/tui.hpp .github/workflows/ci.yml
```

### Step 2: Commit Your Changes

Create a commit with a descriptive message:

```bash
git commit -m "Add CI/CD workflow and fix thread safety issues"
```

**Good commit messages:**
- ✅ "Fix division by zero in CPU stats rendering"
- ✅ "Add mutex protection for thread-safe data access"
- ✅ "Update README with build instructions"

**Bad commit messages:**
- ❌ "fix stuff"
- ❌ "updates"
- ❌ "asdf"

### Step 3: Push to GitHub

Push your changes to GitHub:

```bash
git push origin main
```

If this is your first push or the branch doesn't exist remotely:

```bash
git push -u origin main
```

### Step 4: Watch CI/CD Run

After pushing, GitHub Actions automatically starts:

1. **Go to your GitHub repository** in a web browser
2. **Click the "Actions" tab** at the top
3. **You'll see your workflow running** with a yellow dot (in progress) or green checkmark (success) or red X (failed)

### Step 5: View Results

Click on the workflow run to see details:

- **Each job** (build-linux, build-macos) shows as a separate step
- **Click on a job** to see:
  - Which step is running
  - Build logs
  - Test results
  - Any errors

**Green checkmark** ✅ = Everything passed!
**Red X** ❌ = Something failed (check the logs)

## Understanding the CI Workflow

Our `.github/workflows/ci.yml` does this:

```yaml
on:
  push:
    branches: [ main, master, develop ]
  pull_request:
    branches: [ main, master, develop ]
```

This means CI runs when:
- You push to `main`, `master`, or `develop` branches
- Someone opens a pull request to these branches

**Jobs:**
- `build-linux`: Builds on Ubuntu
- `build-macos`: Builds on macOS

Each job:
1. Checks out code
2. Installs dependencies (CMake, build tools)
3. Sets up vcpkg
4. Configures CMake
5. Builds the project
6. Runs tests

## Common Scenarios

### Scenario 1: First Time Setting Up

If you haven't pushed this repository to GitHub yet:

```bash
# Create a new repository on GitHub (via web interface)
# Then connect it:

git remote add origin https://github.com/YOUR_USERNAME/TBM.git
git branch -M main
git push -u origin main
```

### Scenario 2: CI Fails

If CI shows a red X:

1. **Click on the failed job** to see logs
2. **Look for error messages** (usually at the bottom)
3. **Fix the issue locally**:
   ```bash
   # Test locally first
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
   cmake --build build
   cd build && ctest
   ```
4. **Commit and push again**:
   ```bash
   git add .
   git commit -m "Fix: [describe the fix]"
   git push
   ```

### Scenario 3: Only Test Locally (Skip CI)

Sometimes you want to test without triggering CI:

```bash
# Make changes but don't push
# Or push to a different branch
git checkout -b feature-branch
git push origin feature-branch
# CI won't run (unless you open a PR)
```

### Scenario 4: View CI Status Badge

Add a status badge to your README:

```markdown
![CI](https://github.com/YOUR_USERNAME/TBM/workflows/CI/badge.svg)
```

This shows the latest CI status directly in your README.

## Best Practices

### 1. Test Locally Before Pushing

```bash
# Always test before pushing
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
cmake --build build
cd build && ctest
```

### 2. Make Small, Focused Commits

Instead of:
```bash
git commit -m "Lots of changes"
```

Do:
```bash
git commit -m "Fix thread safety in TUI rendering"
git commit -m "Add mutex protection for process list"
```

### 3. Check CI Before Merging

Always wait for CI to pass (green checkmark) before:
- Merging pull requests
- Tagging releases
- Deploying

### 4. Read CI Logs When It Fails

The logs tell you exactly what went wrong. Common issues:
- Missing dependencies
- Compilation errors
- Test failures
- Missing files

## Troubleshooting

### CI Times Out

If CI takes too long:
- Check if dependencies are downloading slowly
- Consider caching vcpkg packages (advanced)

### Tests Fail in CI But Pass Locally

This usually means:
- Different environment (different OS version)
- Missing dependencies
- Path issues

**Solution**: Check CI logs and replicate the environment locally.

### "Workflow file not found"

Make sure `.github/workflows/ci.yml` exists and is committed:
```bash
git add .github/workflows/ci.yml
git commit -m "Add CI workflow"
git push
```

## Advanced: Customizing CI

You can modify `.github/workflows/ci.yml` to:
- Add Windows builds
- Run code coverage
- Deploy artifacts
- Run linters
- Send notifications

Example: Add code coverage
```yaml
- name: Generate Coverage
  run: |
    cmake --build build --target coverage
```

## Quick Reference

```bash
# 1. Check status
git status

# 2. Add changes
git add .

# 3. Commit
git commit -m "Your message"

# 4. Push (triggers CI)
git push origin main

# 5. Check CI status
# Go to: https://github.com/YOUR_USERNAME/TBM/actions
```

## Need Help?

- **GitHub Actions Docs**: https://docs.github.com/en/actions
- **Workflow Syntax**: https://docs.github.com/en/actions/using-workflows/workflow-syntax-for-github-actions
- **Check your workflow file**: `.github/workflows/ci.yml`

---

**Remember**: CI/CD is your safety net. It catches problems before they reach production! 🚀

