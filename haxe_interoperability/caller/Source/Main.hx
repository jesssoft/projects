package;

import cpp.Lib;

import openfl.display.Sprite;
import openfl.utils.ByteArray;

import openfl.text.Font;
import openfl.text.TextField;
import openfl.text.TextFormat;

import haxe.io.Bytes;
import haxe.io.BytesData;

class Main extends Sprite {

	public function new () {
		super ();

		var byteArray = getByteArray();

		trace('Length: ${byteArray.length}');
		byteArray.position = 0;
		while (byteArray.bytesAvailable > 0) {
			trace('Byte: ${byteArray.readByte()}');
		}
		trace('Finished!');
		displayOk();
	}

	private function displayOk() : Void {
		var format = new TextFormat ("Katamotz Ikasi", 40, 0x7A0026);
		var textField = new TextField ();

		textField.defaultTextFormat = format;
		textField.embedFonts = true;
		textField.selectable = false;

		textField.x = 50;
		textField.y = 50;
		textField.width = 200;

		textField.text = "Success!";

		addChild (textField);
	}

	public static function getByteArray(): ByteArray {
		// call native_bytearray()
		// transform the result from cpp to a ByteArray

		var obj = native_bytearray();
		var bytes = @:privateAccess new Bytes(obj.length, obj.b);
		var byteArray = ByteArray.fromBytes(bytes);

		return byteArray;
	}

#if windows
	private static var native_bytearray = Lib.load("native_extension-19",
						"native_bytearray",
						0);
#else
	private static var native_bytearray = Lib.load("native_extension",
						"native_bytearray",
						0);
#end
}
