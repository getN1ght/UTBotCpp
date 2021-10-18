/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2021. All rights reserved.
 */

#ifndef UNITTESTBOT_COMPILECOMMAND_H
#define UNITTESTBOT_COMPILECOMMAND_H


#include "BaseCommand.h"

#include "utils/path/FileSystemPath.h"
#include <forward_list>
#include <list>
#include <optional>
#include <string>
#include <vector>

namespace utbot {
    class CompileCommand : public BaseCommand {
    private:
        iterator sourcePath;
        iterator compiler;
        iterator output;

    public:
        CompileCommand() = default;

        CompileCommand(CompileCommand const &other);

        CompileCommand &operator=(CompileCommand const &other);

        CompileCommand(CompileCommand &&other) noexcept;

        CompileCommand &operator=(CompileCommand &&other) noexcept;

        CompileCommand(std::vector<std::string> arguments, fs::path directory, fs::path sourcePath);

        friend void swap(CompileCommand &a, CompileCommand &b) noexcept;

        [[nodiscard]] fs::path getSourcePath() const;

        void setSourcePath(fs::path sourcePath);

        [[nodiscard]] fs::path getCompiler() const;

        void setCompiler(fs::path compiler);

        [[nodiscard]] fs::path getOutput() const override;

        [[nodiscard]] bool isArchiveCommand() const override;

        void setOutput(fs::path output);

        void removeGccFlags();

        void filterCFlags();

        void removeIncludeFlags();
    };
}


#endif //UNITTESTBOT_COMPILECOMMAND_H