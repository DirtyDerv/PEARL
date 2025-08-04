# BG Reader - Git Branching Strategy

## Branch Structure

### `main` Branch
- **Purpose**: Stable, tested releases only
- **Protection**: No direct commits - only merge from development via PR
- **Current Version**: v0.01
- **Usage**: Production-ready code, releases, documentation

### `development` Branch 
- **Purpose**: Active development and testing
- **Protection**: All new features and iterations
- **Current Version**: v0.01+ (incrementing)
- **Usage**: Daily development, version increments, experimental features

## Development Workflow

### 1. Feature Development
```bash
# Work on development branch
git checkout development
git pull origin development

# Make changes, increment version
# Edit version.h: 0.01 -> 0.02
# Build and test

# Commit with version
git add .
git commit -m "v0.02: [Feature description]"
git push origin development
```

### 2. Release Process
When ready for stable release:
```bash
# On development branch - prepare release
# Update version to next major (e.g., 0.XX -> 1.00)
git commit -m "v1.00: Release candidate"

# Create pull request: development -> main
# After review and testing, merge to main
# Tag the release
git tag v1.00
git push origin v1.00
```

### 3. Version Numbering Strategy

**Development Branch**:
- Increment by 0.01 for each iteration
- 0.01, 0.02, 0.03, 0.04... 

**Main Branch (Releases)**:
- Major version numbers: 1.00, 2.00, 3.00
- Only stable, tested features

## Current Status
- **main**: v0.01 (Stable - Version system + Big font display)
- **development**: v0.01 (Ready for next iteration)

## Benefits
1. **Safety**: Main branch always stable for hardware testing
2. **Freedom**: Development branch for experimentation 
3. **History**: Clear version progression in both branches
4. **Collaboration**: Easy to review changes via pull requests
5. **Rollback**: Can always return to last stable main version

## Next Steps
All future development happens on `development` branch until ready for next major release to `main`.
