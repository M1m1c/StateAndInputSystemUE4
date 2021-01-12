// © 2018, Ludvig Baummann Olsson. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class StateAndInputSystemEditorTarget : TargetRules
{
	public StateAndInputSystemEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.AddRange( new string[] { "StateAndInputSystem" } );
	}
}
