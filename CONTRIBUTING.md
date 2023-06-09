# Contributing to viewmd

This project is primarily written in C. GUI elements are written in XML. You don't have to be an expert in C to contribute. Find a simple bug to work on and send a merge request.

## Filing a bug

Please file bugs for issues, enhancements and features at the [bug tracker](https://github.com/wilbura009/viewmd/issues).

Create a Merge request and I’d be happy to review your patch and help you get it merged.

## Coding style

All contributions to viewmd should follow its coding style. The style can mostly be inferred from the code. 

* Code should be indented with spaces only - **No tabs ever**
* Use 2 space for each indentation level

This project will try to follow GTK coding style (mostly). Read it online at https://gitlab.gnome.org/GNOME/gtk/-/blob/main/docs/CODING-STYLE.md

The following exceptions apply:

1. Maximum line width is 100 characters.
2. Header files should use `#pragma once` as include guard.
3. Include the full URL to the bug report in commit messages so that they can be accessed from local git log (or mirror repositories).

```txt
settings: Fix a memory leak

g_get_current_dir() returns a newly allocated string.
It should be freed using g_free()

Closes: <https://github.com/wilbura009/viewmd/issues/#>
```

## Commits

Use the following to guide commit messages.

```
type [optional scope]: description

[optional body]

[optional footer(s)]
```

The commit type can include the following:

| Type       | Description                                                                                                                                |
|------------|--------------------------------------------------------------------------------------------------------------------------------------------|
| `feat`     | a new feature is introduced with the changes                                                                                               |
| `fix`      | a bug fix has occurred                                                                                                                     |
| `chore`    | changes that do not relate to a fix or feature and don't modify src or test files (for example updating dependencies)                      |
| `refactor` | refactored code that neither fixes a bug nor adds a feature                                                                                |
| `docs`     | updates to documentation such as the README or other markdown files                                                                        |
| `style`    | changes that do not affect the meaning of the code, likely related to code formatting such as white-space, missing semi-colons, and so on. |
| `test`     | including new or correcting previous tests                                                                                                 |
| `perf`     | performance improvements                                                                                                                   |
| `ci`       | continuous integration related                                                                                                             |
| `build`    | changes that affect the build system or external dependencies                                                                              |
| `revert`   | reverts a previous commit                                                                                                                  |

The commit type subject line should be all lowercase with a character limit to encourage succinct descriptions.

The `[optional body]` should be used to provide further detail that cannot fit within the character limitations of the subject line description.

It is also a good location to utilize `BREAKING CHANGE: description to note the reason for a breaking change within the commit.`

The `[optional footer]`. We use the footer to link the JIRA story that would be closed with these changes for example: `Closes D2IQ-[JIRA #]`.

------------------------------

## References

- https://www.freecodecamp.org/news/how-to-write-better-git-commit-messages/ 
