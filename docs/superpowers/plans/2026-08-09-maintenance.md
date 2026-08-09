# STM32 手写表达式计算器维护计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 在不改变 STM32F407ZGTX、触摸屏和识别模型接口的前提下，降低运行时风险并让工程可以在新环境中复现构建。

**Architecture:** 保留 STM32CubeIDE/CubeMX 工程结构；将 CNN 推理的临时张量改为固定静态工作区，将识别结果格式化和边界检查收敛到 `pt.c`；删除提交中的本机编译产物，新增忽略规则、版本记录和构建说明。

**Tech Stack:** C, STM32 HAL, STM32CubeIDE, GNU Arm Embedded Toolchain, Git.

---

### Task 1: 清理工程交付内容并建立版本记录

**Files:**
- Create: `.gitignore`
- Create: `CHANGELOG.md`
- Modify: `README.md`
- Delete from Git tracking: `Debug/`, `Release/`

- [ ] 添加 STM32CubeIDE 的 `Debug/`、`Release/` 及本地配置忽略规则。
- [ ] 从 Git 索引移除已提交的对象文件、依赖文件和带本机绝对路径的自动生成 makefile。
- [ ] 更新 README，说明目标芯片、导入方式、构建方式、硬件假设和当前版本限制。
- [ ] 记录 `v1.1.0` 的维护内容、验证结果和未完成的硬件实测事项。

### Task 2: 固定推理工作区并增加输入保护

**Files:**
- Modify: `Core/Src/pt.c`
- Modify: `Core/Src/pt.h`

- [ ] 用文件内静态数组替换 `forward()` 中的 5 次动态分配，保持各层形状和模型权重不变。
- [ ] 增加 `malloc` 不再参与推理的注释和工作区大小说明，避免嵌入式堆碎片化。
- [ ] 为表达式字符写入、分隔线写入和结果格式化增加固定上限，确保坏输入不会越界。
- [ ] 对除数为零给出可显示的错误结果，不执行未定义的整数除法。

### Task 3: 复现验证并发布

**Files:**
- Modify: `docs/superpowers/plans/2026-08-09-maintenance.md`

- [ ] 使用仓库已有构建入口或 STM32CubeIDE 生成的 make 目标进行完整构建；若本机无 Arm 工具链，明确记录阻塞证据。
- [ ] 对修改后的 C 源码执行可用的语法/静态检查，并检查 Git diff、忽略规则和仓库状态。
- [ ] 创建带版本号的提交 `v1.1.0`，推送到远程 `main`，再用远程引用确认提交已到达。
