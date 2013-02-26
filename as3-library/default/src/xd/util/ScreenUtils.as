package xd.util {
import flash.display.Screen;
import flash.geom.Rectangle;

public class ScreenUtils {
	
	public static function find(wd:uint, ht:uint):Screen {
		var ss:Array = Screen.screens;
		for each (var s:Screen in ss) {
			var b:Rectangle = s.bounds;
			if(b.width == wd && b.height == ht) { 
			return s;
			}
		}
	return null;
	}

	
	
	
	
	}
}