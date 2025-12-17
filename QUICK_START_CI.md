# Quick Start: Push Changes and Trigger CI/CD

## Your Repository
Your GitHub repository is: `https://github.com/ckakkar/Terminal-Based-System-Monitor.git`

## Step-by-Step Commands

### 1. Check what changed
```bash
git status
```

### 2. Add all your changes
```bash
git add .
```

### 3. Commit with a message
```bash
git commit -m "Add CI/CD workflow and improve code quality"
```

### 4. Push to GitHub (this triggers CI!)
```bash
git push origin main
```

### 5. Watch CI run
Open in browser: https://github.com/ckakkar/Terminal-Based-System-Monitor/actions

## That's it! 🎉

GitHub will automatically:
- Build your code on Linux
- Build your code on macOS  
- Run all tests
- Show you green ✅ or red ❌

## Next Steps After Pushing

1. **Go to GitHub**: https://github.com/ckakkar/Terminal-Based-System-Monitor
2. **Click "Actions" tab** (top menu)
3. **See your workflow running** - it takes 2-5 minutes
4. **Click on the workflow** to see detailed logs

## If Something Fails

1. Click on the failed job (red X)
2. Scroll down to see error messages
3. Fix the issue locally
4. Push again:
   ```bash
   git add .
   git commit -m "Fix: [describe what you fixed]"
   git push origin main
   ```

## Pro Tips

- **Test locally first**: Run `cmake --build build && ctest` before pushing
- **Small commits**: Make focused commits, not huge ones
- **Check CI before merging**: Always wait for green ✅

---

**Need more details?** See [CI_CD_GUIDE.md](CI_CD_GUIDE.md)

