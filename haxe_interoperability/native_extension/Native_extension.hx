package;


import lime.system.CFFI;
import lime.system.JNI;


class Native_extension {
	
	
	public static function sampleMethod (inputValue:Int):Int {
		
		#if android
		
		var resultJNI = native_extension_sample_method_jni(inputValue);
		var resultNative = native_extension_sample_method(inputValue);
		
		if (resultJNI != resultNative) {
			
			throw "Fuzzy math!";
			
		}
		
		return resultNative;
		
		#else
		
		return native_extension_sample_method(inputValue);
		
		#end
		
	}
	
	
	private static var native_extension_sample_method = CFFI.load ("native_extension", "native_extension_sample_method", 1);
	
	#if android
	private static var native_extension_sample_method_jni = JNI.createStaticMethod ("org.haxe.extension.Native_extension", "sampleMethod", "(I)I");
	#end
	
	
}