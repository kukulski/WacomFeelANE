package xd.touch
{
	import flash.display.NativeWindow;
	import flash.geom.Rectangle;

	public class DeviceManager
	{
		private static const _key:Object = {};
		private static var _instance:DeviceManager;
		public var bounds:Rectangle;
		public var fullscreen:Boolean;
		private var drivers:Array;
		
		public var sendCollated:Boolean = false;
		public var sendIndividual:Boolean = true;
		public var overlay:ITouchOverlay;
		
		private var _windows:Vector.<NativeWindow> = new Vector.<NativeWindow>;
		public static function get instance():DeviceManager {
			if(!_instance) {
				_instance = new DeviceManager(_key);
				_instance.drivers = [new WacomANE
				];
			}
			return _instance;				
		}
		public function DeviceManager(key:Object)
		{
			if(key != _key) throw new Error("singleton. pls use DeviceManager.instance");
			_instance = this;
		}
		

		public function get windows():Vector.<NativeWindow> { return _windows;}
		public function registerWindow(w:NativeWindow):void {
			unregisterWindow(w);
			_windows.push(w);
		}
		public function unregisterWindow(w:NativeWindow):void {
			var idx:int = _windows.indexOf(w);
			if(idx != -1)
				_windows.splice(idx,1);
		}
		

	
	}
}