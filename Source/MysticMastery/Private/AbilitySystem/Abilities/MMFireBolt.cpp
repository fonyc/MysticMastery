// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MMFireBolt.h"

FString UMMFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = -GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
		//Title
		"<Title>FIRE BOLT</>\n\n"

		//Spell Details (Level + ManaCost + Cooldown)
		"<Small>Level: </><Level>%d</>\n"
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

		//Spell Description (Damage)
		"<Default>Launches a bolt of fire, exploding on impact and dealing: </>"
		"<Damage>%d</><Default> fire damage with a chance to burn.</>\n\n"),
		Level, ManaCost, Cooldown, ScaledDamage);
	}
	return FString::Printf(TEXT(
	//Title
	"<Title>FIRE BOLT</>\n\n"

	//Spell Details (Level + ManaCost + Cooldown)
	"<Small>Level: </><Level>%d</>\n"
	"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
	"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

	//Spell Description (ProjectileNumber + Damage)
	"<Default>Launches %d bolts of fire, exploding on impact and dealing: </>"
	"<Damage>%d</><Default> fire damage with a chance to burn.</>\n\n"),
		
	Level, ManaCost, Cooldown, FMath::Min(Level, ProjectileNumber), ScaledDamage);
}

FString UMMFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = -GetManaCost(Level);
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
	//Title
	"<Title>NEXT LEVEL</>\n\n"

	//Spell Details (Level + ManaCost + Cooldown)
	"<Small>Level: </><Level>%d</>\n"
	"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
	"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"

	//Spell Description (ProjectileNumber + Damage)
	"<Default>Launches %d bolts of fire, exploding on impact and dealing: </>"
	"<Damage>%d</><Default> fire damage with a chance to burn.</>\n\n"),
	
	Level, ManaCost, Cooldown, FMath::Min(Level, ProjectileNumber), ScaledDamage);
}