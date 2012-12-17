package xd.touch.wacom
{
	import flash.events.TouchEvent;
	import xd.touch.BaseContact;
	
	public class WacomContact extends BaseContact
	{
		public var state:int;

		private const kTouchTypeMap:Vector.<String> = Vector.<String>([
			TouchEvent.TOUCH_BEGIN, TouchEvent.TOUCH_MOVE, TouchEvent.TOUCH_END, null]);
		
		public  function adjust():void {
			touchType = kTouchTypeMap[state];
		}	
	}
}