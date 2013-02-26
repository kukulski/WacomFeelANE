package xd.touch
{

	//import flash.events.EventDispatcher;
	import flash.events.EventDispatcher;
	import flash.geom.Rectangle;
	import flash.text.TextField;
	
	import xd.touch.wacom.PenInput;

	public class WacomANE  extends EventDispatcher
	{
		public var dispatch:Boolean = true;
		
		public function WacomANE()
		{

		
		}

		
		
		public function get tabletBounds():Rectangle {
			
			return null;
		}
		
		public var tf:TextField; 

		
		
		public function get penData():PenInput {
			return null;
		}
	}
}