@file:Suppress("UnstableApiUsage")

package org.utbot.cpp.clion.plugin.ui.wizard.steps

import com.intellij.ui.dsl.builder.COLUMNS_LARGE
import com.intellij.ui.dsl.builder.bindText
import com.intellij.ui.dsl.builder.columns
import com.intellij.ui.dsl.builder.panel
import org.utbot.cpp.clion.plugin.settings.UTBotSettingsModel
import org.utbot.cpp.clion.plugin.ui.wizard.UTBotBaseWizardStep
import org.utbot.cpp.clion.plugin.utils.commandLineEditor

class BuildOptionsStep(private val settingsModel: UTBotSettingsModel) : UTBotBaseWizardStep() {
    override fun createUI() {
        addHtml("media/options_wizard_text.html")
        panel {
            row("Relative path to Build directory") {
                textField().bindText(settingsModel.projectSettings::buildDirRelativePath).columns(COLUMNS_LARGE)
            }
        }.addToUI()
        addHtml("media/cmake_options.html")
        panel {
            row {
                commandLineEditor({ settingsModel.projectSettings.cmakeOptions },
                    { value: String -> settingsModel.projectSettings.cmakeOptions = value })
            }
        }.addToUI()
    }
}
