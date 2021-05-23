

# VSCode Mac setup

Add `"C_Cpp.default.includePath"` to `settings.json`:

```javascript
{
    "editor.tabSize": 2,
    "workbench.tree.indent": 16,
    "explorer.autoReveal": false,
    "C_Cpp.commentContinuationPatterns": [

        "/**"
    ],
    "C_Cpp.default.includePath": [
        "/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino/**",
        "/Applications/Arduino.app/Contents/Java/hardware/tools/avr/avr/include/**",
        "/Applications/Arduino.app/Contents/Java/hardware/tools/avr/lib/gcc/avr/5.4.0/include/**",
        "/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/variants/standard/**",
        "/Applications/Arduino.app/Contents/Java/libraries/**",
        "${workspaceFolder}/libraries/**",
        "${workspaceFolder}/**"
    ],
"C_Cpp.intelliSenseEngine": "Tag Parser"
}
```