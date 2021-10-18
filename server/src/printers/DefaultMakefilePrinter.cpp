/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2021. All rights reserved.
 */

#include "DefaultMakefilePrinter.h"

#include "utils/Copyright.h"

namespace printer {
using StringUtils::stringFormat;

DefaultMakefilePrinter::DefaultMakefilePrinter() {
    writeCopyrightHeader();
}

void DefaultMakefilePrinter::comment(std::string const &message) {
    ss << stringFormat("# %s\n", message);
}

void DefaultMakefilePrinter::declareVariable(std::string const &name, std::string const &value) {
    ss << stringFormat("%s = %s\n", name, value);
}

void DefaultMakefilePrinter::declareAction(const string &name) {
    ss << name << "\n";
}
void DefaultMakefilePrinter::declareInclude(const string &otherMakefileName) {
    ss << stringFormat("include %s\n", otherMakefileName);
}

void DefaultMakefilePrinter::writeCopyrightHeader() {
    ss << Copyright::GENERATED_MAKEFILE_HEADER << NL;
}

}