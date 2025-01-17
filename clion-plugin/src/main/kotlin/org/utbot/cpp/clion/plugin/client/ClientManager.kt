package org.utbot.cpp.clion.plugin.client

import com.intellij.openapi.Disposable
import com.intellij.openapi.components.Service
import com.intellij.openapi.project.Project
import com.intellij.openapi.util.Disposer
import org.utbot.cpp.clion.plugin.client.channels.GTestLogChannelImpl
import org.utbot.cpp.clion.plugin.client.channels.LogChannel
import org.utbot.cpp.clion.plugin.client.channels.ServerLogChannelImpl
import kotlin.random.Random
import org.utbot.cpp.clion.plugin.listeners.ConnectionSettingsListener
import org.utbot.cpp.clion.plugin.utils.logger
import org.utbot.cpp.clion.plugin.utils.projectLifetimeDisposable

@Service
class ClientManager(val project: Project) : Disposable {
    private val clientId = generateClientID()
    private val loggingChannels = listOf<LogChannel>(GTestLogChannelImpl(project), ServerLogChannelImpl(project))
    var client: Client = Client(project, clientId, loggingChannels)
        private set

    init {
        Disposer.register(this, project.projectLifetimeDisposable)
        subscribeToEvents()
    }

    private fun subscribeToEvents() {
        with(project.messageBus.connect(project.projectLifetimeDisposable)) {
            subscribe(ConnectionSettingsListener.TOPIC, object : ConnectionSettingsListener {
                override fun connectionSettingsChanged(newPort: Int, newServerName: String) {
                    if (newPort != client.port || newServerName != client.serverName) {
                        project.logger.trace { "Connection settings changed. Setting up new client." }
                        restartClient()
                    }
                }
            })
        }
    }

    fun restartClient() {
        client.dispose()
        client = Client(project, clientId, loggingChannels)
    }

    override fun dispose() = client.dispose()

    private fun generateClientID(): String {
        fun createRandomSequence() = (1..RANDOM_SEQUENCE_LENGTH)
            .joinToString("") { Random.nextInt(0, RANDOM_SEQUENCE_MAX_VALUE).toString() }

        return "${(System.getenv("USER") ?: "user")}-${createRandomSequence()}"
    }

    companion object {
        const val RANDOM_SEQUENCE_MAX_VALUE = 10
        const val RANDOM_SEQUENCE_LENGTH = 5
    }
}
