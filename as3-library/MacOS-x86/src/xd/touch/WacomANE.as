package xd.touch
{

	//import flash.events.EventDispatcher;
	import flash.events.Event;
	import flash.events.EventDispatcher;
	import flash.events.StatusEvent;
	import flash.external.ExtensionContext;
	import flash.text.TextField;
	import flash.utils.ByteArray;

	public class WacomANE  extends EventDispatcher
	{
		private var   _ExtensionContext:ExtensionContext;
		private var _ba:ByteArray = new ByteArray;
		
		public function WacomANE()
		{
			try
			{
				_ExtensionContext = ExtensionContext.createExtensionContext("xd.touch.WacomANE",null);
				_ExtensionContext.addEventListener(StatusEvent.STATUS, gotEvent);				
				trace("wacom ANE initialized (and this is the updated one)");
			}
			catch (e:Error)
			{
				trace(e);
			}
			
		}
		
		public var tf:TextField; 
		private function gotEvent(event:StatusEvent):void
		{
			trace(event);
			
			if(tf) tf.appendText(event.toString());
			dispatchEvent(new Event(event.code));
			trace(event);
		}

		public function data():ByteArray {
			return _ExtensionContext.call("getData") as ByteArray;
		}	
		
		public function penData():ByteArray {
			return _ExtensionContext.call("getPenData") as ByteArray;
		}
		
		
		public function init():* {
			return(_ExtensionContext.call("init"));
		}
		
		
		
	}
}