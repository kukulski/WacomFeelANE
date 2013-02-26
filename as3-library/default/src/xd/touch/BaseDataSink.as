package xd.touch
{
	import flash.utils.ByteArray;
	import flash.utils.Endian;

	public class BaseDataSink
	{
		protected var endian:String = Endian.LITTLE_ENDIAN;
		public function BaseDataSink()
		{
		}
		
		public function read(d:ByteArray):void {
			
		}
		public function dispatch():void {
			
		}
		
	}
}