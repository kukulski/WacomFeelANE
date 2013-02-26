package xd.touch
{
	import flash.display.InteractiveObject;
	import flash.events.Event;
	import flash.events.TouchEvent;
	
	public class PenEvent extends TouchEvent
	{
	//	public static const PEN_HOVER:String = "penHover";
		private var _buttons:int;
		
		public function PenEvent(type:String, bubbles:Boolean=true, cancelable:Boolean=false, touchPointID:int=0, isPrimaryTouchPoint:Boolean=false, 
								 localX:Number=NaN, localY:Number=NaN, sizeX:Number=NaN, sizeY:Number=NaN, pressure:Number=NaN, relatedObject:InteractiveObject=null, 
								 ctrlKey:Boolean=false, altKey:Boolean=false, shiftKey:Boolean=false, commandKey:Boolean=false, controlKey:Boolean=false,
								 timeStamp:Number=NaN, intent:String = "unknown", buttons:int=0)
		{
			super(type, bubbles, cancelable, touchPointID, isPrimaryTouchPoint, localX, localY, sizeX, sizeY, pressure, relatedObject, ctrlKey, altKey, shiftKey, commandKey, controlKey,timeStamp,intent);
			_buttons = buttons;
		}
		
		public override function clone():Event {
			return new PenEvent(type,bubbles,cancelable, touchPointID,
				isPrimaryTouchPoint,localX, localY, sizeX, sizeY, pressure, relatedObject, ctrlKey, altKey, shiftKey, commandKey,controlKey, timestamp, touchIntent,_buttons);
		}
		
		public override function isToolButtonDown(index:int):Boolean {
			return 0 != ((_buttons >> index) & 2);	 // ignore the bottom bit
		}
		
		public override function stopImmediatePropagation():void { BaseInput._stopped = true; super.stopImmediatePropagation()}
		public override function stopPropagation():void { BaseInput._stopped = true; super.stopPropagation()}

	}
}