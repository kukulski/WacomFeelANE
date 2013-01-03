package xd.touch
{

	//import flash.events.EventDispatcher;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.StatusEvent;
	import flash.external.ExtensionContext;
	import flash.geom.Rectangle;
	import flash.text.TextField;
	import flash.utils.ByteArray;
	
	import xd.touch.wacom.PenInput;
	import xd.touch.wacom.WacomContact;

	public class WacomANE  extends EventDispatcher
	{
		private var   _ExtensionContext:ExtensionContext;
		private var _ba:ByteArray = new ByteArray;
		private static const touchCount:uint = 20;
		private var _dispatchMap:Object;
		public var dispatch:Boolean = true;
		private var bounds:Rectangle;
		
		public function WacomANE()
		{
			try
			{
				_ExtensionContext = ExtensionContext.createExtensionContext("xd.touch.WacomANE",null);
				_ExtensionContext.addEventListener(StatusEvent.STATUS, gotEvent);
				
					_ExtensionContext.call("init", makeExchangeObject());
						trace("wacom ANE initialized (and this is the updated one)");
			}
			catch (e:Error)
			{
				trace(e);
			}
		
			_dispatchMap = { pen:onPen, touch:onTouch, device:onDevice};
		
		}

		
		
		private function makeExchangeObject():* {
			return {
				pen: new PenInput, 
				toolMap: [],
				touches: [0, makeTouches(touchCount)]
			}
		}
		
		
		public function get tabletBounds():Rectangle {
			
			return bounds;
		}
		
		private function makeTouches(count:uint):Vector.<WacomContact> {
			var rval:Vector.<WacomContact> = new Vector.<WacomContact>;
			for (var i:int = 0 ; i < count; i++) {
				rval.push(new WacomContact());
			}
			return rval;
		}
		
		public var tf:TextField; 
		private function gotEvent(event:StatusEvent):void
		{
			var code:Function = _dispatchMap[event.code];
			if(code && dispatch) code(event);
		}
		
		private function onPen(e:*):void {
			var pen:PenInput = _ExtensionContext.call("getPenData") as PenInput;
			pen.dispatch();
		}

		private function onTouch(e:*):void {
			var countPair:Array = _ExtensionContext.call("getTouchData") as Array;
			var contacts:Vector.<WacomContact> = countPair[1] as Vector.<WacomContact>;
			var count:uint = countPair[0] as uint;
			
			for(var i:int = 0; i < count; i++) {
			//	trace(i, contacts[i].id);
				contacts[i].dispatch();
			}
		}
		private function onDevice(e:StatusEvent):void {
			
			var boundsString:String = e.level;
			var args:Array = boundsString.split(',');
			bounds = new Rectangle(args[0], args[1], args[2],args[3]);

			dispatchEvent(new Event("Bounds"));
		}
		
		
		
		public function get penData():PenInput {
			
			var pen:PenInput = _ExtensionContext.call("getPenData") as PenInput;
			return pen;
		}
	}
}