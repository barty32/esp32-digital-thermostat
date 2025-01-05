
#include <Arduino.h>
#include <vector>

class Terminal {

  protected:

	Stream &stream = Serial;

  private:

	String received;

	int cursorPos = 0;
	bool escapeContinues = false;
	String escape;

	std::function<void(Terminal&, const String&, const String&)> commandHandler = nullptr;

	void executeEscapeSequence() {
		if(escape == "[A") {
			//stream.print("up");
		}
		else if(escape == "[B") {
			//stream.print("down");
		}
		else if(escape == "[C") { //right
			if(cursorPos < received.length()) {
				cursorPos++;
				stream.print('\e');
				stream.print(escape);
			}
		}
		else if(escape == "[D") { //left
			if(cursorPos > 0) {
				cursorPos--;
				stream.print('\e');
				stream.print(escape);
			}
		}
		else {
			stream.print(escape);
		}
	}

	bool readEscapeSequence(char c) {
		if(c < 0x20) {//invalid
			escapeContinues = false;
			return false;
		}
		if(escape.isEmpty()) {
			if(c == '[') {
				escape += (char)c;
				return true;
			}
			escapeContinues = false;
			return false;
		}
		if(c >= 0x40 && c <= 0x7E) { //final byte
			escape += (char)c;
			escapeContinues = false;
			this->executeEscapeSequence();
			return true;
		}
		escape += (char)c;
		return true;
	}

	bool readCommand() {
		if(!stream.available()) {
			return false;
		}

		while(true) {
			int c = stream.read();
			if(c < 0) {
				//EOF
				return false;
			}
			if(escapeContinues) {
				if(this->readEscapeSequence(c)) {
					continue;
				}
			}
			if(c == 0x08 || c == 0x7F) { // backspace
				if(cursorPos > 0) {
					received.remove(cursorPos - 1);
					stream.print("\b \b");
					cursorPos--;
					// stream.print((char)c);
				}
				else {
					stream.print((char)0x07); // bell
				}
				continue;
			}
			if(c == '\n' || c == '\r') {//end of command
				stream.println();
				cursorPos = 0;
				break;
			}
			if(c == 0x1B) {//escape sequence
				//stream.print((char)0x1B);
				escapeContinues = true;
				escape = "";
				continue;
			}

			// if(c < 0x20) {//other control characters
			// 	continue;
			// }
			if(isPrintable(c)){
				stream.print((char)c);
				if(received.length() == cursorPos) received += (char)c;
				//received = received.substring(0, cursorPos) + (char)c + received.substring(cursorPos);
				else received.setCharAt(cursorPos, (char)c);
				cursorPos++;
			}
		}
		return true;
	}

  public:

	void init() {

	}

	void update() {
		if(!this->readCommand()) {
			return;
		}

		received.trim();
		if(received.isEmpty()) {
			this->handleCommand("", "");
			return;
		}

		const int space = received.indexOf(' ');
		const String command = space == -1 ? received : received.substring(0, space);
		const String params = space == -1 ? "" : received.substring(space + 1);
		//const int space = received.indexOf(' ');
		// String command;// = space == -1 ? received : received.substring(0, space);
		// std::vector<String> params; // = space == -1 ? "" : received.substring(space + 1);
		// char quote = 0;
		// for(int i = 0; i < received.length(); i++) {
		// 	char c = received[i];
		// 	if(c == quote) {
		// 		quote = 0;
		// 		continue;
		// 	}
		// 	if(received[i] == ' ') {
		// 		command = received.substring(0, i);
		// 		payload = received.substring(i + 1);
		// 		break;
		// 	}

		// }
		
		received = "";

		this->handleCommand(command, params);
	}

	Stream& getStream() {
		return stream;
	}

	void setCommandHandler(std::function<void(Terminal&, const String&, const String&)> handler) {
		this->commandHandler = handler;
	}

	virtual void handleCommand(const String &command, const String &params) {
		if(this->commandHandler) {
			this->commandHandler(*this, command, params);
			return;
		}
		if(command.isEmpty()) {
			//do nothing
		}
		else if(command == "clear") {
			stream.print("\e[2J\e[H");
			//stream.print((char)0x0C);
		}
		else if(command == "ping") {
			stream.println("pong");
		}
		else {
			stream.println("Unknown command: '" + command + "'. Use 'help' for a list of commands.");
		}

		this->prompt();
	}

	virtual void prompt() {
		stream.print("thermostat> ");
	}
};
