# CCGL
Common Cross-platform Geographic-computing Library



## Subtree

CCGL use CMake for cross-platform build. The [cmake](https://github.com/lreis2415/cmake) repository is adopted as subtree.

+ Add subtree (for administrator only)

  ```
  git remote add -f cmake git@github.com:lreis2415/cmake.git
  git subtree add --prefix=cmake cmake master --squash
  ```

  ​