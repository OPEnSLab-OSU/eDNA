# Getting Started

We are using git submodule as an internal dependency management. This allows branch selection stright from the parent repo as well as the ability to directly commiting to the sub repo. Submodule is downloaded to `lib/Framework` to take advantage of PlatformIO's build system to find the right symbols.

Learn more: [Git Submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules)

## Relevant Commands

```shell
git submodule update --remote --rebase
git submodule add <remote>
git checkout --recurse-submodule <submodule>
```

## Start Here

### Setup

First navigate to develop branch in eDNA-Server local repo

```shell
git config push.recurseSubmodules check
git config submodule.recurse true
git reset --hard
git pull
```

### Initializing Submodule

```shell
git submodule init
git submodule update
git submodule foreach "git checkout develop"
```

You should be able to compile at this point. If so stop here, else please let me know.

### Pulling Changes From Upstream

```shell
git submodule update --remote --rebase
```

### Branch Naming Convention
- (no prefix) - version release or main branch
- (feature) - creating a new feature based on a feature request
- (experimental) - created for testing out something
- (bugfix) - created for fixing a non-critical issue with the code
- (hotfix) - created for fixing a critical issue with code or an issue with a particular sampler
