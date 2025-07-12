
@file:OptIn(ExperimentalMaterial3Api::class)

package com.example.interiorlightcontrol

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import kotlinx.coroutines.*
import org.json.JSONObject
import java.net.HttpURLConnection
import java.net.URL
import androidx.compose.material3.CenterAlignedTopAppBar
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight

// Main activity that launches the Jetpack Compose UI
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Request location permission for WiFi scanning (required on some Android versions)
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
            != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(
                this,
                arrayOf(Manifest.permission.ACCESS_FINE_LOCATION),
                100
            )
        }

        // Set up the Jetpack Compose UI
        setContent {
            var isDarkTheme by remember { mutableStateOf(false) }         // Tracks current theme
            var wifiStatus by remember { mutableStateOf("Disconnected") } // WiFi connection status

            MaterialTheme(
                colorScheme = if (isDarkTheme) darkColorScheme() else lightColorScheme()
            ) {
                Surface {
                    Scaffold(
                        topBar = {
                            TopAppBar(
                                title = {
                                    Box(
                                        modifier = Modifier
                                            .fillMaxWidth()
                                            .padding(horizontal = 16.dp)
                                    ) {
                                        // App title centered in the top app bar
                                        Text(
                                            text = "Interior Light Control",
                                            fontSize = 20.sp,
                                            textAlign = TextAlign.Center,
                                            modifier = Modifier
                                                .align(Alignment.Center)
                                                .padding(top = 35.dp)
                                        )

                                        // WiFi connection status icon on the left
                                        Row(
                                            verticalAlignment = Alignment.CenterVertically,
                                            modifier = Modifier.align(Alignment.CenterStart)
                                        ) {
                                            val wifiColor = if (wifiStatus == "Connected") MaterialTheme.colorScheme.onSurface else MaterialTheme.colorScheme.error

                                            Icon(
                                                painter = painterResource(id = R.drawable.ic_wifi),
                                                contentDescription = "WiFi Status",
                                                tint = wifiColor,
                                                modifier = Modifier.size(22.dp)
                                            )
                                        }

                                        // Theme toggle icon on the right
                                        IconButton(
                                            onClick = { isDarkTheme = !isDarkTheme },
                                            modifier = Modifier.align(Alignment.CenterEnd)
                                        ) {
                                            Image(
                                                painter = painterResource(
                                                    id = if (isDarkTheme) R.drawable.ic_sun else R.drawable.ic_moon
                                                ),
                                                contentDescription = "Toggle Theme"
                                            )
                                        }
                                    }
                                },
                                navigationIcon = {},
                                actions = {}
                            )
                        }
                    ) { innerPadding ->
                        Box(modifier = Modifier.padding(innerPadding)) {
                            LightSensorScreen(
                                onWifiStatusChanged = { connected ->
                                    wifiStatus = if (connected) "Connected" else "Disconnected"
                                }
                            )
                        }
                    }
                }
            }
        }
    }
}

// Composable function that handles all sensor-related UI and logic
@Composable
fun LightSensorScreen(onWifiStatusChanged: (Boolean) -> Unit) {
    var mode by remember { mutableStateOf("Loading...") }
    var relay by remember { mutableStateOf("Loading...") }
    var light by remember { mutableStateOf("Loading...") }
    var rotary by remember { mutableStateOf("Loading...") }
    var override by remember { mutableStateOf("Loading...") }
    var error by remember { mutableStateOf(false) }

    // Sends a GET command to the specified endpoint
    fun sendCommand(endpoint: String) {
        CoroutineScope(Dispatchers.IO).launch {
            try {
                val url = URL("http://192.168.1.1/$endpoint")
                val conn = url.openConnection() as HttpURLConnection
                conn.connectTimeout = 3000
                conn.readTimeout = 3000
                conn.requestMethod = "GET"
                conn.responseCode
                conn.disconnect()
            } catch (e: Exception) {
                Log.e("LIGHT_APP", "Command failed: $endpoint", e)
            }
        }
    }

    // Periodically fetch sensor data from the CC3200 every 3 seconds
    LaunchedEffect(Unit) {
        while (true) {
            withContext(Dispatchers.IO) {
                try {
                    val url = URL("http://192.168.1.1/sensor")
                    val conn = url.openConnection() as HttpURLConnection
                    conn.connectTimeout = 3000
                    conn.readTimeout = 3000
                    conn.requestMethod = "GET"
                    if (conn.responseCode == 200) {
                        val response = conn.inputStream.bufferedReader().readText()
                        val json = JSONObject(response)
                        mode = json.optString("mode", "N/A")
                        relay = json.optString("relay", "N/A")
                        light = json.optInt("light", -1).toString()
                        rotary = json.optInt("rotary", -1).toString()
                        override = json.optString("override", "no")
                        error = false
                        onWifiStatusChanged(true)
                    } else {
                        error = true
                        onWifiStatusChanged(false)
                    }
                    conn.disconnect()
                } catch (e: Exception) {
                    error = true
                    onWifiStatusChanged(false)
                }
            }
            delay(3000)
        }
    }

    Surface(modifier = Modifier.fillMaxSize()) {
        if (error) {
            // Display error message when unable to connect
            Box(modifier = Modifier.fillMaxSize(), contentAlignment = Alignment.Center) {
                Column(horizontalAlignment = Alignment.CenterHorizontally) {
                    Text("Failed to connect", style = MaterialTheme.typography.titleLarge)
                    Text("Ensure you're connected to LightControlAP", style = MaterialTheme.typography.bodyMedium)
                }
            }
        } else {
            // Main UI layout when connected
            Column(
                modifier = Modifier
                    .padding(24.dp)
                    .fillMaxWidth(),
                verticalArrangement = Arrangement.spacedBy(16.dp),
                horizontalAlignment = Alignment.CenterHorizontally
            ) {
                // Display current mode with colored background
                val modeColor = if (mode.lowercase() == "auto") Color(0xFF81C784) else Color(0xFF90CAF9)
                Text(
                    text = "Mode: ${mode.uppercase()}",
                    color = Color.White,
                    modifier = Modifier
                        .background(modeColor, shape = RoundedCornerShape(12.dp))
                        .padding(horizontal = 16.dp, vertical = 8.dp)
                )

                // Relay ON/OFF display and toggle
                val isOverrideActive = override == "yes"

                Column(horizontalAlignment = Alignment.CenterHorizontally) {
                    Text("Relay", style = MaterialTheme.typography.titleMedium)

                    Row(
                        verticalAlignment = Alignment.CenterVertically,
                        horizontalArrangement = Arrangement.spacedBy(12.dp),
                    ) {
                        Text(
                            text = if (relay == "on") "ON" else "OFF",
                            color = if (relay == "on") Color(0xFF43A047) else Color(0xFFD32F2F),
                            fontSize = 16.sp
                        )

                        Switch(
                            checked = relay == "on",
                            onCheckedChange = { isChecked ->
                                if (isOverrideActive) {
                                    val newState = if (isChecked) "on" else "off"
                                    sendCommand("relay?state=$newState")
                                }
                            },
                            enabled = isOverrideActive
                        )
                    }
                }

                // Parse and evaluate light level from sensor reading
                val lightValue = light.toIntOrNull() ?: 0
                val (lightCondition, lightColor) = when (lightValue) {
                    in 0..100 -> "Dark" to Color(0xFF222831)
                    in 101..1000 -> "Dim" to Color(0xFF393E46)
                    in 1001..2500 -> "Normal" to Color(0xFF00ADB5)
                    in 2501..3500 -> "Bright" to Color(0xFF03DAC5)
                    in 3501..4095 -> "Very Bright" to Color(0xFFFFC107)
                    else -> "Unknown" to Color.Gray
                }

                // Card showing light value and descriptive condition
                Card(
                    colors = CardDefaults.cardColors(containerColor = lightColor),
                    modifier = Modifier
                        .fillMaxWidth(0.9f)
                        .padding(vertical = 8.dp),
                    elevation = CardDefaults.cardElevation(4.dp)
                ) {
                    Column(
                        modifier = Modifier
                            .fillMaxWidth()
                            .padding(12.dp),
                        horizontalAlignment = Alignment.CenterHorizontally
                    ) {
                        Text("Light Level: $lightValue", style = MaterialTheme.typography.titleMedium, color = Color.White, textAlign = TextAlign.Center, modifier = Modifier.fillMaxWidth())
                        Text("Condition: $lightCondition", style = MaterialTheme.typography.bodyMedium, color = Color.White, textAlign = TextAlign.Center, modifier = Modifier.fillMaxWidth())
                    }
                }

                // Display rotary sensor value
                Text(
                    text = "Rotary: $rotary",
                    style = MaterialTheme.typography.bodyLarge,
                    fontWeight = FontWeight.Bold
                )

                // Relay ON/OFF override button
                Button(onClick = {
                    val newState = if (relay == "on") "off" else "on"
                    sendCommand("relay?state=$newState")
                }) {
                    Text(if (relay == "on") "Turn Relay OFF (Override)" else "Turn Relay ON (Override)")
                }

                // Mode switch button (manual <-> auto)
                Button(onClick = {
                    val newMode = if (mode == "manual") "auto" else "manual"
                    sendCommand("mode?state=$newMode")
                }) {
                    Text(if (mode == "manual") "Switch to Auto Mode" else "Switch to Manual Mode")
                }

                // Display car image at bottom
                Image(
                    painter = painterResource(id = R.drawable.car_image),
                    contentDescription = "Car Image",
                    modifier = Modifier.size(200.dp)
                )
            }
        }
    }
}
