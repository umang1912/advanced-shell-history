### HOW DOES THIS WORK? ###

This program operates on the assumption that your shell gives you the ability
to execute a custom operation before a prompt is redrawn on your terminal.

Bash provides the [PROMPT\_COMMAND](http://tldp.org/HOWTO/Bash-Prompt-HOWTO/x264.html) environment variable while zsh gives us the
[precmd](http://zsh.sourceforge.net/Doc/Release/Functions.html#Hook-Functions) environment function.  Both are repurposed by this program to log the
previous command into a local database before each new prompt is displayed.

Many people have never heard of this feature or found a great use for it.
However, if you currently DO use this feature, you can still use this program.
The code renames your existing hook and invokes it after saving the previous
command.

```
\0/ - AWESOME!!!
 Y
/ \
```