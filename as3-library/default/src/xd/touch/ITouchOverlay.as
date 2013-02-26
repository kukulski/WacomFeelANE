package xd.touch
{
	import flash.display.Stage;

	public interface ITouchOverlay
	{
		function unhook():void;
		function update(s:Stage, type:String, x:Number, y:Number, id:int):void;
		
	}
}