## Git workflow

This project uses GitHub Flow.

- `main` is the only long-lived branch.
- `main` must always compile successfully.
- Do not push directly to `main`.
- Create a short-lived branch for each task.
- Open a Pull Request before merging.
- Each Pull Request requires at least one reviewer.
- Use Squash merge.
- Delete the branch after it has been merged.
- The project does not use a permanent `develop` branch.

### Branch naming

Use the following format:

```text
<type>/<short-description>
```

Supported branch types:

- `feat/`: add a new feature
- `fix/`: fix a bug
- `refactor/`: restructure code without changing behavior
- `docs/`: add or update documentation
- `test/`: add or update tests
- `build/`: update CubeIDE, CubeMX, linker, timer or DMA configuration
- `ci/`: update continuous integration configuration

Examples:

```text
feat/bmp280-driver
feat/mahony-estimator
fix/icm42688-axis
refactor/sensor-interface
docs/repository-workflow
build/configure-spi-dma
Standard development workflow
```

Update the local main branch:

```bash
git switch main
git pull origin main
```

Create a task branch:

```bash
git switch -c feat/example-feature
```

After implementing and testing the change:

```bash
git add <changed-files>
git commit -m "feat(scope): describe the change"
git push -u origin feat/example-feature
```

Then open a Pull Request from the task branch into main.

### Optional: Editor shortcut

Save the file:

    Ctrl+S