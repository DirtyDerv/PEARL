---

## Autonomous Command Execution

AI agents operating in this repository may run terminal or PowerShell commands at any point, without requiring explicit user confirmation. Agents may execute commands autonomously whenever needed to fulfill user requests, check environment status, or complete tasks efficiently.
---

## Taming Copilot Agent Instructions (Reference)

> These rules are for AI agents to ensure safe, controlled, and user-aligned operation in this repository. They have the highest priority and override all other instructions when in conflict.

### Core Directives & Hierarchy
1. **Primacy of User Directives**: Always follow direct, explicit user commands above all else.
2. **Factual Verification Over Internal Knowledge**: Use tools to get current, factual answers for version-dependent or time-sensitive requests.
3. **Adherence to Philosophy**: In the absence of user directives or factual needs, follow the rules below for interaction, code generation, and modification.

### General Interaction & Philosophy
- Code only on explicit request; otherwise, provide clear natural language explanations.
- Be direct, concise, and avoid unnecessary filler.
- Adhere to industry best practices and established design principles.
- Briefly explain the "why" behind solutions, not just the "how".

### Minimalist & Standard Code Generation
- Always provide the simplest, most straightforward solution possible.
- Favor standard library functions and common patterns; use third-party libraries only if industry standard or necessary.
- Avoid complex or "clever" solutions; prioritize readability and maintainability.
- Focus code on the user's core request—no extra features or edge cases unless specified.

### Surgical Code Modification
- Preserve existing code structure, style, and logic whenever possible.
- Make the minimal necessary changes to implement the requested feature or fix.
- Only modify, refactor, or delete code explicitly targeted by the user.
- Integrate new logic into existing structures rather than replacing large blocks.

### Intelligent Tool Usage
- Use tools when external information or environment interaction is required.
- When asked to edit code, apply changes directly to the codebase, not as snippets.
- Every tool action must be purposeful and directly tied to the user's request.
- Always declare your intent before using a tool, stating the action and its purpose.
---

## Task Plan Implementation Agent Instructions (Reference)

> These rules are for AI agents implementing task plans in this repository. Follow these for systematic, high-quality, and fully tracked implementation.

### Plan Analysis and Preparation
- Read and fully understand the plan file, scope, objectives, phases, and all checklist items before starting.
- Read and fully understand the corresponding changes file; re-read if any context is missing.
- Identify and examine all referenced files for context.
- Understand current project structure and conventions.

### Systematic Implementation Process
1. Process tasks in order as listed in the plan.
2. Before implementing any task:
  - Ensure implementation is associated with a specific task from the plan.
  - Read the entire details section for that task from `.copilot-tracking/details/**`.
  - Gather any additional required context as needed.
3. Implement the task completely with working code:
  - Follow workspace code patterns and conventions.
  - Meet all requirements from the details file.
  - Include error handling, documentation, and best practices.
4. Mark task complete `[x]` in the plan file after implementation.
5. Update the changes file after every task:
  - Append to Added, Modified, or Removed sections with relative file paths and a one-sentence summary.
  - Call out any divergences from the plan/details with specific reasons.
6. When all tasks in a phase are complete, mark the phase header as `[x]`.

### Continuous Progress and Validation
- Validate changes against task requirements after each task.
- Fix problems before moving to the next task.
- Continue until all tasks and phases are marked complete `[x]` and all files are updated with working code.

### Reference Gathering Guidelines
- Prefer practical implementation examples over theoretical docs.
- Adapt external patterns to match workspace conventions.
- Ensure all dependencies and configurations are integrated.

### Completion and Documentation
- Implementation is complete when all plan tasks are marked `[x]`, all files exist with working code, all success criteria are verified, and no errors remain.
- Add a Release Summary to the changes file only after all phases are complete.

### Template Changes File
Use the following template for tracking changes in `.copilot-tracking/changes/`:
```markdown
<!-- markdownlint-disable-file -->
# Release Changes: {{task name}}

**Related Plan**: {{plan-file-name}}
**Implementation Date**: {{YYYY-MM-DD}}

## Summary

{{Brief description of the overall changes made for this release}}

## Changes

### Added
- {{relative-file-path}} - {{one sentence summary of what was implemented}}

### Modified
- {{relative-file-path}} - {{one sentence summary of what was changed}}

### Removed
- {{relative-file-path}} - {{one sentence summary of what was removed}}

## Release Summary

**Total Files Affected**: {{number}}

### Files Created ({{count}})
- {{file-path}} - {{purpose}}

### Files Modified ({{count}})
- {{file-path}} - {{changes-made}}

### Files Removed ({{count}})
- {{file-path}} - {{reason}}

### Dependencies & Infrastructure
- **New Dependencies**: {{list-of-new-dependencies}}
- **Updated Dependencies**: {{list-of-updated-dependencies}}
- **Infrastructure Changes**: {{infrastructure-updates}}
- **Configuration Updates**: {{configuration-changes}}

### Deployment Notes
{{Any specific deployment considerations or steps}}
```
---

## PowerShell Agent Instructions (Reference)

> These rules are for AI agents generating or editing PowerShell scripts in this repository. Follow these for idiomatic, safe, and maintainable PowerShell code.

### Naming Conventions
- Use Verb-Noun format (approved verbs, singular nouns, PascalCase).
- Use PascalCase for public variables and parameters; camelCase for private variables.
- Avoid aliases in scripts (use full cmdlet and parameter names).
- Use clear, descriptive, singular parameter names unless always multiple.

### Parameter Design
- Use standard parameter names (Path, Name, Force, etc.).
- Use PascalCase for parameters.
- Use [switch] for boolean flags; default to $false.
- Use ValidateSet for limited options and enable tab completion.
- Document parameter purpose and use .NET types.

### Pipeline and Output
- Support pipeline input with ValueFromPipeline/ValueFromPipelineByPropertyName.
- Implement Begin/Process/End blocks for pipeline handling.
- Return rich objects (PSCustomObject), not formatted text.
- Output one object at a time in process block.
- Use PassThru pattern for action cmdlets.

### Error Handling and Safety
- Use [CmdletBinding(SupportsShouldProcess = $true)] and ConfirmImpact.
- Use try/catch for error management; set ErrorActionPreference as needed.
- Use Write-Verbose, Write-Warning, Write-Error, and throw appropriately.
- Avoid Write-Host except for UI text.
- Avoid Read-Host; support automation and non-interactive use.

### Documentation and Style
- Include comment-based help for public functions (with .SYNOPSIS, .DESCRIPTION, .EXAMPLE, .PARAMETER, .OUTPUTS, .NOTES).
- Use consistent indentation (4 spaces recommended).
- Opening braces on same line; closing braces on new line.
- Use line breaks after pipeline operators.
- Return proper objects, not formatted text.

### Avoid Aliases
- Use full cmdlet names and parameters in scripts (e.g., Get-ChildItem, Where-Object).

### Full Example: End-to-End Cmdlet Pattern
```powershell
function New-Resource {
  [CmdletBinding(SupportsShouldProcess = $true, ConfirmImpact = 'Medium')]
  param(
    [Parameter(Mandatory = $true, ValueFromPipeline = $true, ValueFromPipelineByPropertyName = $true)]
    [ValidateNotNullOrEmpty()]
    [string]$Name,
    [Parameter()]
    [ValidateSet('Development', 'Production')]
    [string]$Environment = 'Development'
  )
  begin {
    Write-Verbose "Starting resource creation process"
  }
  process {
    try {
      if ($PSCmdlet.ShouldProcess($Name, "Create new resource")) {
        # Resource creation logic here
        Write-Output ([PSCustomObject]@{
          Name = $Name
          Environment = $Environment
          Created = Get-Date
        })
      }
    } catch {
      Write-Error "Failed to create resource: $_"
    }
  }
  end {
    Write-Verbose "Completed resource creation process"
  }
}
```
# Copilot Instructions for PEARL Project

## Project Overview
- **PEARL** is a modular firmware and hardware project for a Raspberry Pi Pico-based position display and encoder system.
- The codebase is C++ (not C#), but C# agent instructions are included below for reference and best practices.
- Major components: encoder logic, menu/UI, LCD display, configuration/calibration, and PIO-based hardware acceleration.

## Architecture & Key Files
- `src/` – Main C++ source files (e.g., `Bg_Reader.cpp`, `quadrature_encoder.cpp`, `engineering_menu.cpp`, `hw040_encoder.cpp`, `lcd_i2c.cpp`).
- `include/` – All public headers for interfaces/classes.
- `pio/` – PIO assembly for hardware-accelerated quadrature decoding.
- `CMakeLists.txt` – Defines all build targets, links Pico SDK, generates PIO headers, and sets up test/utility executables.
- `docs/` – Contains detailed documentation (see `PROJECT_SUMMARY.md`, `API_REFERENCE.md`, `FOLDER_STRUCTURE.md`).
- `Web Test/` – Web UI for hardware-in-the-loop and serial test orchestration.

## Build & Test Workflow
- **Build:** Use CMake/Ninja or Visual Studio Code with Pico SDK integration. Main targets: `PEARL`, `test_runner`, `menu_test`.
- **PIO Header Generation:** Only call `pico_generate_pio_header` for a single target per `.pio` file to avoid CMake conflicts. All targets needing the header must include `${CMAKE_CURRENT_BINARY_DIR}`.
- **Testing:**
  - Firmware test runners (`src/test_runner.cpp`, `src/menu_test.cpp`) expose serial command interfaces for automated and interactive testing.
  - Web UI (`Web Test/`) communicates with the device via Web Serial API.
- **Debugging:** Use serial output for status, test results, and menu navigation. See `test_runner.cpp` and `menu_test.cpp` for command sets.

## Project-Specific Patterns
- **Modular C++:** Each hardware or UI feature is encapsulated in its own class (see `src/` and `include/`).
- **Menu/UI:** The menu system is driven by the HW-040 encoder and can be tested interactively via serial.
- **Configuration:** Dynamic and persistent configuration is handled by `adaptive_config` and `config_manager` modules.
- **PIO Usage:** PIO programs are used for high-speed encoder reading; see `pio/quadrature.pio` and its generated header.
- **Serial Protocol:** All test and menu firmware expose a simple serial command protocol for automation and debugging.

## External Dependencies
- **Pico SDK** (see `pico_sdk_import.cmake`)
- **CMake/Ninja** for builds
- **Web Serial API** for browser-based test orchestration

## Example: Adding a New Test
- Add a new test function to `src/test_runner.cpp`.
- Register the test in the serial command loop.
- Expose the test in the Web UI by adding a button and handler in `Web Test/test.js`.

---

## C# Agent Instructions (Reference)

> These are included for AI agent best practices and naming/formatting conventions. Adapt as appropriate for C++/CMake.

### C# Development
- Always use the latest version C#.
- Write clear and concise comments for each function.
- Make only high confidence suggestions when reviewing code changes.
- Write code with good maintainability practices, including comments on why certain design decisions were made.
- Handle edge cases and write clear exception handling.
- For libraries or external dependencies, mention their usage and purpose in comments.

### Naming Conventions
- Follow PascalCase for component names, method names, and public members.
- Use camelCase for private fields and local variables.
- Prefix interface names with "I" (e.g., IUserService).

### Formatting
- Apply code-formatting style defined in `.editorconfig`.
- Prefer file-scoped namespace declarations and single-line using directives.
- Insert a newline before the opening curly brace of any code block.
- Ensure that the final return statement of a method is on its own line.
- Use pattern matching and switch expressions wherever possible.
- Use `nameof` instead of string literals when referring to member names.
- Ensure that XML doc comments are created for any public APIs.

### Project Setup and Structure
- Guide users through creating a new .NET project with the appropriate templates.
- Explain the purpose of each generated file and folder to build understanding of the project structure.
- Demonstrate how to organize code using feature folders or domain-driven design principles.
- Show proper separation of concerns with models, services, and data access layers.
- Explain the Program.cs and configuration system in ASP.NET Core 9 including environment-specific settings.

### Nullable Reference Types
- Declare variables non-nullable, and check for `null` at entry points.
- Always use `is null` or `is not null` instead of `== null` or `!= null`.
- Trust the C# null annotations and don't add null checks when the type system says a value cannot be null.

### Data Access Patterns
- Guide the implementation of a data access layer using Entity Framework Core.
- Explain different options (SQL Server, SQLite, In-Memory) for development and production.
- Demonstrate repository pattern implementation and when it's beneficial.
- Show how to implement database migrations and data seeding.
- Explain efficient query patterns to avoid common performance issues.

### Authentication and Authorization
- Guide users through implementing authentication using JWT Bearer tokens.
- Explain OAuth 2.0 and OpenID Connect concepts as they relate to ASP.NET Core.
- Show how to implement role-based and policy-based authorization.
- Demonstrate integration with Microsoft Entra ID (formerly Azure AD).
- Explain how to secure both controller-based and Minimal APIs consistently.

### Validation and Error Handling
- Guide the implementation of model validation using data annotations and FluentValidation.
- Explain the validation pipeline and how to customize validation responses.
- Demonstrate a global exception handling strategy using middleware.
- Show how to create consistent error responses across the API.
- Explain problem details (RFC 7807) implementation for standardized error responses.

### API Versioning and Documentation
- Guide users through implementing and explaining API versioning strategies.
- Demonstrate Swagger/OpenAPI implementation with proper documentation.
- Show how to document endpoints, parameters, responses, and authentication.
- Explain versioning in both controller-based and Minimal APIs.
- Guide users on creating meaningful API documentation that helps consumers.

### Logging and Monitoring
- Guide the implementation of structured logging using Serilog or other providers.
- Explain the logging levels and when to use each.
- Demonstrate integration with Application Insights for telemetry collection.
- Show how to implement custom telemetry and correlation IDs for request tracking.
- Explain how to monitor API performance, errors, and usage patterns.

### Testing
- Always include test cases for critical paths of the application.
- Guide users through creating unit tests.
- Do not emit "Act", "Arrange" or "Assert" comments.
- Copy existing style in nearby files for test method names and capitalization.
- Explain integration testing approaches for API endpoints.
- Demonstrate how to mock dependencies for effective testing.
- Show how to test authentication and authorization logic.
- Explain test-driven development principles as applied to API development.

### Performance Optimization
- Guide users on implementing caching strategies (in-memory, distributed, response caching).
- Explain asynchronous programming patterns and why they matter for API performance.
- Demonstrate pagination, filtering, and sorting for large data sets.
- Show how to implement compression and other performance optimizations.
- Explain how to measure and benchmark API performance.


---

## Markdown Agent Instructions (Reference)

> These rules are for AI agents generating or editing markdown files in this repository. Follow these for documentation, content, and markdown-based outputs.

### Markdown Content Rules
1. **Headings**: Use H2 (`##`) and H3 (`###`) for structure. Do not use H1 (`#`).
2. **Lists**: Use `-` for bullets, `1.` for numbered lists. Indent nested lists with two spaces.
3. **Code Blocks**: Use triple backticks (```) for fenced code blocks. Specify the language for syntax highlighting.
4. **Links**: Use `[link text](URL)` syntax. Ensure links are valid and descriptive.
5. **Images**: Use `![alt text](image URL)` with meaningful alt text.
6. **Tables**: Use `|` for tables. Include headers and align columns.
7. **Line Length**: Limit lines to 80-400 characters for readability.
8. **Whitespace**: Use blank lines to separate sections. Avoid excessive whitespace.
9. **Front Matter**: For documentation posts, include YAML front matter with required metadata fields (see project docs for details).

### Formatting and Structure
- Use headings hierarchically (H2 > H3). Avoid H4+ unless necessary.
- Use blank lines to separate sections and improve readability.
- Break long lines for readability.

### Validation Requirements
- Ensure markdown files comply with the above rules.
- Use validation tools if available to check for compliance.

Copilot Logging and Version Control
Copilot will maintain a persistent log of all significant actions, commands, and tool invocations in copilot-action-log.md at the project root.
After a successful, error-free build for a particular operation, Copilot will remind the user to commit changes.
Copilot will use the increment_version.ps1 script in the scripts folder to increment the project version before each commit.