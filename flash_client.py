Import("env")

# Multiple actions
env.AddCustomTarget(
    name="flash_client",
    dependencies=None,
    actions=[
		"cd ./client && npm install",
		"cd ./client && npm run build",
		"mkdir ./data/client",
		"cp ./client/dist/index.html.gz ./data/client/",
        "pio run --target buildfs",
        "pio run --target uploadfs"
    ],
    title="Flash Web Client",
    description="Build and upload the web client to the ESP32",
)