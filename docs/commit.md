# Commit Guidelines

## Rules

- Commits must follow the Conventional Commits format:
  `type(scope): description`.
- Allowed commit types:
  - `feat`: Introduces a new feature.
  - `fix`: Fixes a bug.
  - `refactor`: Restructures code without altering behavior.
  - `docs`: Documentation changes only.
  - `test`: Adds or updates tests.
  - `chore`: Build system, tooling, or maintenance tasks.
- Scope must reflect the affected directory or component.
- Descriptions must be lowercase.
- Descriptions must use the imperative mood.
- Commit messages must not end with a period.

## Examples

```txt
feat(bar): add foo support
fix(bar/baz): handle null qux
refactor(foo): simplify bar logic
chore: update build script
```
