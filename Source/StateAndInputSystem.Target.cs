// © 2018, Ludvig Baummann Olsson. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class StateAndInputSystemTarget : TargetRules
{
	public StateAndInputSystemTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;

		ExtraModuleNames.AddRange( new string[] { "StateAndInputSystem" } );
	}
}
