import os
import subprocess
Import("env")

# include toolchain paths
# env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

# override compilation DB path
# env.Replace(COMPILATIONDB_PATH=os.path.join("$BUILD_DIR", "compile_commands.json"))
env.Replace(COMPILATIONDB_PATH=os.path.join("$PROJECT_WORKSPACE_DIR", "compile_commands.json"))

if "compiledbtc" not in COMMAND_LINE_TARGETS: #avoids infinite recursion
	subprocess.run(['pio', 'run','-t','compiledbtc'])
