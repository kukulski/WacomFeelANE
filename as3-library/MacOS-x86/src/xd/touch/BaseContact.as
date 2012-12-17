package xd.touch
{
	import com.senocular.ui.VirtualMouse;
	
	import flash.events.Event;
	import flash.geom.Point;
	import flash.utils.ByteArray;

	public class BaseContact extends BaseInput
	{
		public var size:Point = new Point;
		public var id:int;
		public var isPrimary:Boolean;
		
//		protected function get mouse():VirtualMouse {
//			
//			// do immediately after getting the basics working on ntrig
//			return VirtualMouse.getInstance();
//		}
		
		public function BaseContact()
		{
		}
		

		public function toString():String {
			return [
				id,
				onAirDesktop.x.toFixed(2),
				onAirDesktop.y.toFixed(2),
				size.x.toFixed(2),
				size.y.toFixed(2),
				touchType
				].join(' ');
		}
		
		public override function dispatch():void {
		
			findStage(onAirDesktop);
			// no need to call this, since pen events ALWAYS go out as mouse events
			//	super.dispatch();
			
			var e:Event = dist_te(touchType, inActiveWindow, id,size, 1, intent, isPrimary);
			//trace(e);
			//			var mouseType:String = lastButtons ? (buttons ? MouseEvent. : TouchEvent.TOUCH_END) 
			//				: (buttons ? TouchEvent.TOUCH_BEGIN : PenEvent.PEN_HOVER); 
			//			
			
			
		}
	}
}