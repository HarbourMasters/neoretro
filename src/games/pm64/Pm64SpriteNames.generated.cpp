#include "Pm64SpriteNames.h"

namespace Pm64 {
namespace {
const char* const kNpcAlbinoDinoRasters[] = { "Idle1", "Idle2", "Walk1", "Walk2", "Walk3", "Walk4", "Talk", "Fwd1", "Back1", "Run1", "Run2", "Run3", "Charge1", "Charge2", "Pain1", "Pain2", "Dead", "BurntPain1", "BurntPain2", "BurntDead", "Fwd2", "Back2" };
const char* const kNpcAltBanditRasters[] = { "Step1", "Step2", "Step3", "Neutral", "CarryStep1", "CarryStep2", "CarryStep3", "CarryNeutral", "LookUp", "Taunt", "Tackle", "Hurt1", "Idle1", "Idle2", "CarryIdle1", "CarryIdle2", "Hurt2", "BurnHurt1", "BurnHurt2", "BurnStill", "Talk", "SmokeCloud1", "SmokeCloud2", "SmokeCloud3", "SmokeCloud4", "SmokeCloud5", "SmokeCloud6" };
const char* const kNpcArcheologistRasters[] = { "Idle1", "Idle2", "Step1", "Step2", "Step3", "Talk" };
const char* const kNpcBabyBlooperRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12" };
const char* const kNpcBanditRasters[] = { "Step1", "Step2", "Step3", "Neutral", "StepHolding1", "StepHolding2", "StepHolding3", "NeutralHolding", "LookUp", "GrinHolding", "Tackle", "Hurt1", "Idle1", "Idle2", "IdleHolding1", "IdleHolding2", "Hurt2", "BurnHurt1", "BurnHurt2", "BurnStill", "Sleep", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "Laugh1", "Laugh2", "Relief1", "Relief2", "Coin01", "Coin02", "Coin03", "Coin04", "Coin05", "Coin06", "Coin07", "Coin08", "Coin09", "Coin10", "Poof1", "Poof2", "Poof3", "Poof4", "Poof5", "Poof6" };
const char* const kNpcBartenderRasters[] = { "Idle1", "Idle2", "Idle3", "Idle4", "Idle5", "Talk", "StrumGuitar1", "StrumGuitar2", "Step1", "Step2", "Step3" };
const char* const kNpcBattleBombetteRasters[] = { "Neutral", "Step1", "Step2", "Step3", "Step4", "Idle1", "Idle2", "Idle3", "Idle4", "Jump", "Fall", "GearAngled", "LitStill", "LitStep1", "LitStep2", "LitStep3", "Brace", "Hurt1", "Hurt2", "Block", "Talk", "BodySlam", "IdleHappy1", "IdleHappy2", "IdleHappy3", "IdleHappy4", "IdleHappy5", "JumpHappy", "FallHappy", "FuseLit1", "FuseLit2", "FuseLit3", "FuseEnd1", "FuseEnd2", "FuseEnd3", "GearNeutral", "GearSpin1", "GearSpin2", "GearSpin3", "GearSpin4", "GearSpin5", "GearSpin6", "GearSpin7", "Sleep", "Injured1", "Injured2", "BurnHurt1", "BurnHurt2", "GearBurnHurt1", "GearBurnHurt2" };
const char* const kNpcBattleBowRasters[] = { "Neutral", "Step1", "Step2", "Talk1", "Talk2", "PreSlap1", "PreSlap2", "Slap1", "Slap2", "Slap3", "Conceal", "Spook1", "Spook2", "Spook3", "Spook4", "Laugh1", "Laugh2", "Celebrate1", "Celebrate2", "Celebrate3", "Celebrate4", "BurnHurt1", "BurnHurt2", "Hurt1", "Hurt2", "TailDown", "Block", "BurnHurtAlt1", "BurnHurtAlt2", "FanSmack1", "FanSmack2", "LargePatch", "ArmAcrossBody", "NoLeftArm", "Pray", "Injured1", "Injured2", "Injured3", "FanAcrossBody1", "FanAcrossBody2", "ArmHoldingFan1", "ArmHoldingFan2", "SmallPatch" };
const char* const kNpcBattleBowserRasters[] = { "Head", "BodyWalk", "Shell", "LArm", "RArm", "HeadAngry", "LFoot", "RFoot", "LFootStep1", "RFootStep1", "LFootStep2", "RFootStep2", "LFootStep3", "RFootStep3", "HeadTalk1", "HeadTalk2", "HeadMock1", "HeadBlink", "HeadMock3", "HeadTense", "BodyStand", "BodyRotated", "LFootRotated", "RFootRotated", "FootBottom", "LArmBrandish", "RArmBrandish", "RHandBrandish", "LArmRaised", "RArmRaised1", "RArmRaised2", "LArmBent", "RArmBent", "BodyLand", "BurntHead", "BurntBody", "BurntLArm", "BurntRArm", "BurntLFoot", "BurntRFoot", "HeadFireBreath", "DefeatedHead", "DefeatedHeadBlink", "DefeatedHeadTalk1", "DefeatedHeadTalk2", "StarRod", "DefeatedBody", "DefeatedRArm", "DefeatedLFoot", "DefeatedRFoot", "LFist", "LArmFist", "RArmFist", "RFist", "HeadRotated1", "HeadRotated2", "HeadRotated3", "HeadTantrum", "HeadSurprise", "HeadEyesClosed", "HeadLaugh1", "HeadLaugh2", "LArmSwipe", "RFistSwipe", "RArmSwipe", "BodyFwd", "LFootTantrum", "RFootTantrum", "Hair" };
const char* const kNpcBattleClubbaRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21" };
const char* const kNpcBattleEldstarRasters[] = { "Idle1", "Idle2", "Shout" };
const char* const kNpcBattleGoombarioRasters[] = { "Step1", "Step2", "Step3", "Hurt", "Headbonk", "Idle1", "Idle2", "Idle3", "Talk1", "Talk2", "Jump", "Fall", "Celebrate1", "Celebrate2", "PreHeadbonk", "Injured1", "Injured2", "CloseEyes1", "CloseEyes2", "BurnStand", "BurnHurt1", "BurnHurt2", "Think", "Block1", "Block2" };
const char* const kNpcBattleKalmarRasters[] = { "Idle1", "Idle2", "Shout" };
const char* const kNpcBattleKammyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F" };
const char* const kNpcBattleKlevarRasters[] = { "Idle1", "Idle2", "Shout" };
const char* const kNpcBattleKoloradoRasters[] = { "Head", "Idle1", "Idle2", "Panic1", "Panic2", "Shout1", "Shout2", "Injured", "Hurt", "SadHead", "Step1", "Step2", "Step3" };
const char* const kNpcBattleKooperRasters[] = { "Step1", "Step2", "Step3", "Idle1", "Idle2", "Idle3", "Idle4", "Shell1", "Shell2", "Shell3", "Shell4", "Shell5", "Shell6", "Shell7", "Shell8", "Hurt1", "Hurt2", "EnterShell1", "EnterShell2", "EnterShell3", "Talk", "Shout1", "Shout2", "Pleased", "Celebrate1", "Celebrate2", "Celebrate3", "Jump", "Fall", "Toppled1", "Toppled2", "Pray1", "Pray2", "BurnHurt1", "BurnHurt2", "BurnStill", "Think", "Sad" };
const char* const kNpcBattleLakilesterRasters[] = { "CloudIdle1", "CloudIdle2", "CloudIdle3", "CloudExhale", "Vanish1", "Vanish2", "Vanish3", "Vanish4", "Vanish5", "Vanish6", "Vanish7", "Vanish8", "CloudBlock", "CloudInhale", "CloudExtended1", "CloudExtended2", "CloudExtended3", "CloudStrain", "CloudHappy", "CloudHurt", "CloudDizzy", "Spiny", "Idle1", "Idle2", "Idle3", "Hurt1", "Hurt2", "Shout", "Pray", "Injured1", "Injured2", "Happy1", "Happy2", "ReachDown", "HoldSpiny1", "HoldSpiny2", "RaiseArms1", "RaiseArms2", "LeanForward1", "LeanForward2", "Block", "CloudBurnt1", "CloudBurnt2", "Burnt1", "Burnt2" };
const char* const kNpcBattleMamarRasters[] = { "Idle1", "Idle2", "Shout" };
const char* const kNpcBattleMerleeRasters[] = { "Release", "Gather" };
const char* const kNpcBattleMisstarRasters[] = { "Idle1", "Idle2", "Shout" };
const char* const kNpcBattleMuskularRasters[] = { "Idle1", "Idle2", "Shout" };
const char* const kNpcBattleParakarryRasters[] = { "LWingFlap1", "LWingFlap2", "LWingFlap3", "LWingFlap4", "Shell", "EnterShell1", "EnterShell2", "LegsBack", "LegsForward", "Talk", "Celebrate1", "Celebrate2", "Hurt1", "Hurt2", "Lift1", "Lift2", "EnterShell4", "PreDive", "Dive", "PostDive", "RWingFlap1", "RWingFlap2", "RWingFlap3", "RWingFlap4", "RWingFlap5", "LWingFlap5", "Blink1", "Blink2", "RaisedArm", "Pray", "Injured1", "Injured2", "BurnHurt1", "BurnHurt2", "BurnStill" };
const char* const kNpcBattleSkolarRasters[] = { "Idle1", "Idle2", "Shout" };
const char* const kNpcBattleSushieRasters[] = { "Idle1", "Idle2", "Idle3", "OpenMouth1", "OpenMouth2", "Tense1", "Tense2", "Jump", "Fall", "Land", "Squirt1", "Squirt2", "SpitArc1", "SpitArc2", "Hurt1", "Hurt2", "BurnHurt1", "BurnHurt2", "BurnStill", "Injured1", "Injured2", "Injured3", "Injured4", "Happy1", "Happy2", "Think", "Pray", "HoldWater", "Block" };
const char* const kNpcBattleTubbaRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F", "Raster30", "Raster31", "Raster32", "Raster33", "Raster34", "Raster35", "Raster36", "Raster37", "Raster38", "Raster39", "Raster3A", "Raster3B", "Raster3C", "Raster3D", "Raster3E", "Raster3F", "Raster40", "Raster41", "Raster42", "Raster43", "Raster44", "Raster45", "Raster46", "Raster47", "Raster48", "Raster49" };
const char* const kNpcBattleWattRasters[] = { "Idle1", "Idle2", "Idle3", "Talk", "EyesClosed1", "EyesClosed2", "EyesClosed3", "Happy1", "Happy2", "Happy3", "Hurt1", "Hurt2", "Hurt3", "Think1", "Think2", "Think3", "BiggerStrain1", "BiggerStrain2", "BiggerStrain3", "BiggestStrain1", "BiggestStrain2", "Strain1", "Strain2", "Strain3", "BurnHurt1", "BurnHurt2", "BurnHurt3", "BurnStill", "Injured1", "Injured2", "Injured3", "Injured4", "ULBolt", "LLBolt", "URBolt", "LRBolt" };
const char* const kNpcBigLanternGhostRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25" };
const char* const kNpcBillBlasterRasters[] = { "Barrel", "BaseFront", "BaseBack", "BarrelFire1", "BarrelFire2", "BarrelFire3", "BarrelFire4", "BurnedBarrel", "BurnedBaseFront", "BurnedBaseBack" };
const char* const kNpcBlooperRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13" };
const char* const kNpcBobombRasters[] = { "Step1", "Step2", "Step3", "Idle", "FaceFwd", "WaveFwd", "Blink", "Hurt1", "Hurt2", "BurnHurt1", "BurnHurt2", "Neutral", "BurnStill", "Sleep1", "Sleep2", "Wake1", "Wake2", "Boom", "Gear1", "Gear2", "Gear3", "Gear4", "Gear5", "Gear6", "Fuse1", "Fuse2", "Fuse3", "BubbleSleep1", "BubbleSleep2", "BubblePop1", "BubblePop2", "KO", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "Dizzy5", "Dizzy6", "Cower", "BurnedGear1", "BurnedGear2", "BurnedGear3", "BurnedGear4", "BurnedGear5", "BurnedGear6", "StepLit1", "StepLit2", "StepLit3", "Buildup", "CowerLit" };
const char* const kNpcBonyBeetleRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F", "Raster30", "Raster31", "Raster32", "Raster33", "Raster34", "Raster35", "Raster36", "Raster37", "Raster38", "Raster39", "Raster3A", "Raster3B", "Raster3C", "Raster3D", "Raster3E", "Raster3F", "Raster40", "Raster41", "Raster42", "Raster43", "Raster44", "Raster45", "Raster46", "Raster47", "Raster48", "Raster49", "Raster4A", "Raster4B", "Raster4C", "Raster4D", "Raster4E", "Raster4F", "Raster50" };
const char* const kNpcBooRasters[] = { "Step1", "Step2", "Step3", "Step4", "Carry1", "Carry2", "Carry3", "Carry4", "Conceal", "Spook1", "Spook2", "Spook3", "Flail1", "Flail2", "CarryTalk", "Cower1", "Cower2", "Talk1", "Talk2", "Wave1", "Wave2", "Wave3", "Wave4", "Confused1", "Confused2", "Throw1", "Throw2", "Blink", "CarryBlink", "CarryHappy1", "CarryHappy2", "CarryHappy3", "CarryHappy4" };
const char* const kNpcBootlerRasters[] = { "Idle", "Step1", "Step2", "Step3", "Talk", "Panic1", "Panic2", "Panic3", "Flail1", "Flail2", "TapChest", "Quaver1", "Quaver2", "Spook1", "Spook2" };
const char* const kNpcBubulbRasters[] = { "Idle1", "Idle2", "Idle3", "Idle4", "NoseTwitch", "Talk1", "Talk2", "Happy1", "Happy2", "BuriedNeutral", "PopUp1", "PopUp2", "PopUp3", "Hole", "HoleOpen1", "HoleOpen2", "BuriedIdle1", "BuriedIdle2", "HappyGentle1", "HappyGentle2", "HappyGentle3", "HappyTalk1", "HappyTalk2", "DarkStep1", "DarkStep2", "DarkStep3", "Bloom1", "Bloom2", "Bloom3", "Bloom4", "EmbedIdle1", "EmbedIdle2", "EmbedIdle3", "EmbedIdle4", "EmbedIdle5", "Step1", "Step2", "Step3" };
const char* const kNpcBulletBillRasters[] = { "Neutral", "Tense", "Windup1", "Windup2", "Windup3", "Launch1", "Launch2", "Launch3", "Launch4", "Hurt", "Burned", "BurnedPuffy" };
const char* const kNpcBushBasicRasters[] = { "Bush" };
const char* const kNpcBushBlockyRasters[] = { "Bush" };
const char* const kNpcBushDryRasters[] = { "Bush" };
const char* const kNpcBushLeafyRasters[] = { "Bush" };
const char* const kNpcBushMattedRasters[] = { "Bush" };
const char* const kNpcBuzzarRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F", "Raster30", "Raster31", "Raster32", "Raster33", "Raster34", "Raster35", "Raster36", "Raster37", "Raster38", "Raster39", "Raster3A", "Raster3B", "Raster3C", "Raster3D", "Raster3E", "Raster3F", "Raster40", "Raster41", "Raster42", "Raster43", "Raster44", "Raster45", "Raster46", "Raster47", "Raster48", "Raster49", "Raster4A", "Raster4B", "Raster4C", "Raster4D", "Raster4E", "Raster4F", "Raster50", "Raster51" };
const char* const kNpcBuzzyBeetleRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F", "Raster30", "Raster31", "Raster32" };
const char* const kNpcBzzapRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25" };
const char* const kNpcChainChompRasters[] = { "Neutral", "Chain", "Bite", "Hurt1", "Hurt2", "Sleep" };
const char* const kNpcChanRasters[] = { "Idle1", "Idle2", "Idle3", "ToppledIdle1", "ToppledIdle2", "Hurt1", "Hurt2", "Step1", "Step2", "ShellSpin1", "ShellSpin2", "ShellSpin3", "ShellSpin4", "ShellSpin5", "ShellSpin6", "Empty1", "Empty2", "Step3", "EnterShell1", "EnterShell2", "EnterShell3", "Sleep", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "ToppledHurt1", "ToppledHurt2", "ToppledBurnHurt1", "ToppledBurnHurt2" };
const char* const kNpcChanterelleRasters[] = { "Idle1", "Idle2", "Idle3", "Idle4", "Idle5", "Blink", "Sing1", "Sing2", "Sing3", "Talk", "Step1", "Step2", "Step3" };
const char* const kNpcChetRippoRasters[] = { "Neutral", "Blink", "Idle1", "Idle2", "Talk", "Spread1", "Spread2", "Shake1", "Shake2", "Swim1", "Swim2", "Swim3" };
const char* const kNpcChuckQuizmoRasters[] = { "Idle1", "Idle2", "Idle3", "Idle4", "Step1", "Step2", "Talk", "Wrong", "Open1", "Open2", "Open3", "Correct1", "Correct2", "Vanish01", "Vanish02", "Vanish03", "Vanish04", "Vanish05", "Vanish06", "Vanish07", "Vanish08", "Vanish09", "Vanish10", "Vanish11", "Vanish12", "Vanish13", "QuestionIcon1", "QuestionIcon2", "QuestionIcon3", "CorrectIcon1", "CorrectIcon2", "CorrectIcon3", "OpenHat1", "WrongIcon1", "WrongIcon2", "WrongIcon3", "OrangeDot", "OpenHat2", "YellowDot", "OpenHat3", "BlueDot" };
const char* const kNpcCleftRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27" };
const char* const kNpcCoinRasters[] = { "Rotate01", "Rotate02", "Rotate03", "Rotate04", "Rotate05", "Rotate06", "Rotate07", "Rotate08", "Rotate09", "Rotate10" };
const char* const kNpcConductorToadRasters[] = { "Neutral", "Idle", "Step1", "Step2", "Step3", "BlowWhistle1", "BlowWhistle2", "BlowWhistle3", "BlowWhistle4", "BlowWhistle5", "Talk" };
const char* const kNpcCrystalKingRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F", "Raster30", "Raster31", "Raster32", "Raster33", "Raster34", "Raster35", "Raster36", "Raster37" };
const char* const kNpcDarkToadRasters[] = { "Neutral", "Talk1", "Talk2", "Idle1", "Idle2", "Idle3", "Step1", "Step2", "Step3", "Hurt1", "Hurt2", "Dismiss1", "Dismiss2" };
const char* const kNpcDayzeeRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C" };
const char* const kNpcDisguisedMoustafaRasters[] = { "Still", "Talk", "Blink1", "Blink2", "GrabCloak1", "GrabCloak2", "ThrownCloak1", "ThrownCloak2" };
const char* const kNpcDryBonesRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23" };
const char* const kNpcDryiteRasters[] = { "Neutral", "Idle", "Step1", "Step2", "Talk" };
const char* const kNpcDuplighostRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B" };
const char* const kNpcFiceTRasters[] = { "Idle1", "Idle2", "Idle3", "Shiver1", "Shiver2", "Talk", "LookUp", "Read", "Panic1", "Panic2", "Step1", "Step2", "Step3", "Tear1", "Tear2", "Tear3" };
const char* const kNpcFireRasters[] = { "Burn1", "Burn2", "Burn3" };
const char* const kNpcFishmaelRasters[] = { "Neutral", "Idle", "Reel1", "Talk1", "Reel2", "Talk2", "Reel3", "Reel4" };
const char* const kNpcFlyingMagikoopaRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16" };
const char* const kNpcFuzzipedeRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29" };
const char* const kNpcFuzzyRasters[] = { "Neutral", "Step1", "Step2", "Raster03", "Raster04", "Raster05", "Hurt1", "Hurt2", "Stunned1", "Stunned2", "Stunned3", "Stunned4", "Raster0C", "Bite1", "Bite2", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Cry1", "Cry2" };
const char* const kNpcGateFlowerRasters[] = { "Head", "HeadDisgust", "HeadHappy", "HeadEat", "LPetal", "RPetal", "JawPetal", "Stem1", "Stem2", "Stem3", "Stem4" };
const char* const kNpcGeneralGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A" };
const char* const kNpcGeneralGuyBombRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04" };
const char* const kNpcGoomaRasters[] = { "Neutral", "Idle", "Step1", "Step2", "Talk1", "Talk2" };
const char* const kNpcGoomamaRasters[] = { "Idle1", "Idle2", "Step1", "Step2", "Talk1", "Talk2", "Talk3", "Step3" };
const char* const kNpcGoombaRasters[] = { "Step1", "Step2", "Step3", "Hurt1", "Hurt2", "Midair", "Idle1", "Idle2", "Sleep", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "BurnHurt1", "BurnHurt2", "BurnStill", "Tense", "Toadstool", "Mushroom", "XRay" };
const char* const kNpcGoombaBrosRasters[] = { "Step1", "Step2", "Step3", "Midair", "Idle1", "Idle2", "Idle3", "Hurt1", "Hurt2", "Talk1", "Talk2", "CryStep1", "CryStep2", "CryStep3", "CryIdle1", "CryIdle2", "CryIdle3", "CryTalk1", "CryTalk2", "Tear1", "Tear2", "Tear3", "Tear4", "Sleep", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "BurnHurt1", "BurnHurt2", "BurnStill" };
const char* const kNpcGoombaKingRasters[] = { "Body", "Pants", "LEyebrow", "REyebrow", "LMustache", "RMustache", "Crown1", "Crown2", "EyesCrossed1", "EyesCrossed2", "EyesApart", "EyesCrossedOut", "EyesOpen", "MouthOpen", "MouthClose", "MouthFrown", "LFoot1", "RFoot1", "LFoot2", "RFoot2", "MouthHalfOpen", "BurnedBody", "BurnedPants", "BurnedLMustache", "BurnedRMustache", "BurnedCrown1", "BurnedEyes", "BurnedMouthOpen", "BurnedMouthClosed", "BurnedLFoot1", "BurnedRFoot1", "BurnedLFoot2", "BurnedRFoot2", "BurnedLEyebrow", "BurnedREyebrow", "BurnedCrown2", "EyesClosed" };
const char* const kNpcGoombariaRasters[] = { "Step1", "Step2", "Step3", "Hurt", "JumpEyesClosed", "Idle1", "Idle2", "Idle3", "Talk1", "Talk2", "Jump", "Fall", "Celebrate1", "Celebrate2", "LeanOver1", "LeanOver2", "LeanOver3", "Inspect1", "Inspect2", "Blink1", "Blink2", "LookUp1", "LookUp2", "LookUp3", "LookUpTalk1", "SadTalk1", "SadStep1", "SadStep2", "SadStep3", "SadTalk2", "SadTalk3", "LookUpTalk2", "Tear1", "Tear2", "Shock1", "Shock2" };
const char* const kNpcGoompaRasters[] = { "Step1", "Step2", "Step3", "Injured", "Hurt", "Idle1", "Idle2", "Idle3", "Talk1", "Talk2", "Jump", "Fall", "Happy1", "Happy2", "WearyTalk1", "WearyTalk2", "WearyTalk3", "WearyIdle1", "WearyIdle2", "WearyIdle3", "WearyStep1", "WearyStep2", "WearyStep3", "Rummage1", "Rummage2", "Rummage3" };
const char* const kNpcGoompapaRasters[] = { "Idle1", "Idle2", "Step1", "Step2", "Talk1", "Talk2", "Angry", "AngryTalk1", "AngryTalk2", "Shock1", "Shock2", "Step3", "LookUpTalk", "LookUp1", "LookUp2", "SideEye", "TapFoot1", "TapFoot2" };
const char* const kNpcGourmetGuyRasters[] = { "Neutral", "Step1", "Step2", "Idle", "Eat", "SpitOut", "Surprise1", "Surprise2", "Knife", "Fork", "Panic1", "Panic2", "Panic3", "Leap", "Chew1", "Chew2", "Talk", "Inspect1", "Inspect2", "SitIdle1", "SitIdle2" };
const char* const kNpcGrooveGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19" };
const char* const kNpcGulpitRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D" };
const char* const kNpcHammerBrosRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28" };
const char* const kNpcHarryTRasters[] = { "Neutral", "Step1", "Step2", "Step3", "Idle1", "Idle2", "Idle3", "Idle4", "Blink", "Talk" };
const char* const kNpcHeartPlantRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13" };
const char* const kNpcHerringwayRasters[] = { "Neutral", "TalkCarry1", "TalkCarry2", "IdleCarry", "Recieve1", "Recieve2", "WalkCarry1", "WalkCarry2", "WalkCarry3", "RunCarry1", "RunCarry2", "RunCarry3", "Walk1", "Walk2", "Walk3", "Idle", "Run1", "Run2", "Run3", "Talk1", "Talk2" };
const char* const kNpcHuffNPuffRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F", "Raster30", "Raster31", "Raster32", "Raster33", "Raster34", "Raster35", "Raster36", "Raster37" };
const char* const kNpcHurtPlantRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F", "Raster30", "Raster31", "Raster32", "Raster33", "Raster34", "Raster35", "Raster36", "Raster37", "Raster38" };
const char* const kNpcJrTroopaRasters[] = { "Neutral", "Idle", "Step1", "Step2", "Step3", "Talk", "Shout1", "Shout2", "Jump", "Midair", "Fall", "PointTalk", "PointNeutral", "PointTapFoot", "EnterEgg1", "EnterEgg2", "EggNeutral", "EggSqueeze", "Hurt1", "Hurt2", "Panic1", "Panic2", "BurnStill", "Sleep1", "Sleep2", "Sleep3", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "LandAlt", "JumpAlt", "MidairAlt", "Trip1", "Trip2", "Slide", "ScamperBack1", "ScamperBack2", "Defeated1", "Defeated2", "Defeated3", "Defeated4", "Charge1", "Charge2", "Charge3", "Collapse1", "Collapse2", "FallHeadfirstBack", "BurnHurt1", "BurnHurt2", "Flail1", "Flail2", "EggIdle", "OutOfBreath1", "OutOfBreath2", "OutOfBreath3", "TiredTalk1", "TiredTalk2", "StepBack1", "StepBack2", "StepBack3", "ClimbUpBack1", "ClimbUpBack2", "ClimbUpBack3", "ChargeArmsUp1", "ChargeArmsUp2", "ChargeArmsUp3", "ChargeTripped1", "ChargeTripped2", "ChargeTripped3", "PointIdle", "Punch1", "Punch2", "Punch3", "Punch4" };
const char* const kNpcKentCKoopaRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F", "Raster30", "Raster31", "Raster32", "Raster33", "Raster34", "Raster35", "Raster36", "Raster37", "Raster38", "Raster39", "Raster3A", "Raster3B", "Raster3C", "Raster3D", "Raster3E", "Raster3F", "Raster40", "Raster41", "Raster42", "Raster43", "Raster44", "Raster45", "Raster46", "Raster47", "Raster48", "Raster49", "Raster4A", "Raster4B", "Raster4C", "Raster4D", "Raster4E", "Raster4F", "Raster50", "Raster51", "Raster52", "Raster53", "Raster54", "Raster55", "Raster56", "Raster57", "Raster58", "Raster59", "Raster5A", "Raster5B", "Raster5C", "Raster5D" };
const char* const kNpcKoloradoRasters[] = { "Step1", "Step2", "Step3", "Idle1", "Idle2", "Idle3", "Talk", "Panic1", "Panic2", "Panic3", "Yell1", "Yell2", "Shout1", "Shout2", "Fallen1", "Fallen2", "Hurt1", "Hurt2", "IdleSad1", "IdleSad2", "IdleSad3", "TalkSad", "Cry", "StepSad1", "StepSad2", "StepSad3" };
const char* const kNpcKoloradoWifeRasters[] = { "Step1", "Step2", "Step3", "Idle1", "Idle2", "Blink", "Talk" };
const char* const kNpcKoopaRasters[] = { "Step1", "Step2", "Step3", "Idle1", "Idle2", "Talk", "Happy1", "Happy2", "Happy3", "Happy4", "Topple1", "Topple2", "Shell", "Blink", "LeanBack1", "LeanBack2", "LeanBackTalk", "EmbedIdle1", "EmbedIdle2" };
const char* const kNpcKoopaBrosRasters[] = { "Neutral", "Idle1", "Idle2", "CrouchNeutral", "CrouchIdle1", "CrouchIdle2", "Step1", "Step2", "Step3", "ShellNeutral", "ShellSpin1", "ShellSpin2", "ShellSpin3", "ShellSpin4", "ShellSpin5", "Shock1", "Shock2", "Hurt1", "Hurt2", "IdleToppled1", "IdleToppled2", "Talk1", "Talk2", "Talk3", "Leap1", "Leap2", "Leap3", "Leap4", "Land", "IdleTipping1", "IdleTipping2", "IdleTipping3", "ExitShell1", "ExitShell2", "ExitShell3", "LayingDown", "Tipping1", "Tipping2", "Tipping3", "BurnHurt1", "BurnHurt2", "PointForward1", "PointForward2", "ThumbsUp1", "ThumbsUp2", "ThumbsUp3", "Hammer1", "Hammer2", "Blink" };
const char* const kNpcKoopaKootRasters[] = { "Idle1", "Idle2", "Idle3", "Step1", "Step2", "Step3", "Celebrate1", "Celebrate2", "Celebrate3", "HoldCane", "TalkCane", "IdleCane1", "IdleCane2", "RaiseCane" };
const char* const kNpcKoopaTroopaRasters[] = { "Step1", "Step2", "Step3", "Idle1", "Idle2", "Salute", "HurtStill", "Hurt1", "Hurt2", "BurnStill", "ToppleStruggle1", "ToppleStruggle2", "ShellSpin1", "ShellSpin2", "ShellSpin3", "ShellSpin4", "ShellSpin5", "ShellSpin6", "BurnHurt1", "BurnHurt2", "Sleep1", "Sleep2", "Sleep3", "EnterShell1", "EnterShell2", "EnterShell3", "Panic1", "Panic2", "Panic3", "BeginPanic1", "BeginPanic2", "ToppleSleep1", "ToppleSleep2", "ToppleSleep3", "ToppleStill", "Scramble1", "Scramble2", "Scramble3", "ToppleStunned1", "ToppleStunned2", "ToppleStunned3", "ToppleStunned4" };
const char* const kNpcKoopaWithoutShellRasters[] = { "Step1", "Step2", "Step3", "Idle1", "Idle2", "Talk", "Happy1", "Happy2", "Happy3", "Happy4", "Toppled1", "Toppled2", "Blink", "SadStep1", "SadStep2", "SadStep3", "SadIdle1", "SadIdle2", "SadTalk", "CryStep1", "CryStep2", "CryStep3", "CryIdle1", "CryIdle2", "CryTalk1", "CryTalk2", "Tear1", "Tear2" };
const char* const kNpcKoopatrolRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29" };
const char* const kNpcKooperWithoutShellRasters[] = { "Step1", "Step2", "Step3", "Talk", "TalkAngry", "Bow", "Idle1", "Idle2", "Idle3", "Idle4", "Celebrate1", "Celebrate2", "Celebrate3", "StepAngry1", "StepAngry2", "StepAngry3", "Shout1", "Shout2", "Happy", "IdleAngry1", "IdleAngry2", "IdleAngry3", "IdleAngry4", "ShoutAngry1", "ShoutAngry2" };
const char* const kNpcLakiluluRasters[] = { "Neutral", "Step1", "Step2", "Step3", "Step4", "Talk1", "Talk2", "HappyNod1", "HappyNod2", "HappyTalk1", "HappyTalk2", "Panic1", "Panic2", "Panic3", "Panic4", "NeutralInhale", "Shout1", "Shout2", "ConcernedNod1", "ConcernedNod2", "ConcernedTalk1", "ConcernedTalk2", "Lift1", "Lift2", "Throw1", "Throw2", "Throw3", "Throw4", "Spiny" };
const char* const kNpcLakituRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F", "Raster30", "Raster31", "Raster32", "Raster33" };
const char* const kNpcLargePiranhaRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24" };
const char* const kNpcLavaBubbleRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B" };
const char* const kNpcLavaBudRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C" };
const char* const kNpcLavaPiranhaRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16" };
const char* const kNpcLeadersFriendRasters[] = { "Idle1", "Idle2", "Step1", "Step2", "Step3", "Talk", "Bow1", "Bow2", "Plead1", "Plead2", "Plead3", "IdleSit1", "BowSit1", "BowSit2", "TalkSit1", "IdleSit2", "TalkSit2" };
const char* const kNpcLeafRasters[] = { "Still" };
const char* const kNpcLeeRasters[] = { "Idle1", "Idle2", "Idle3", "Idle4", "Step1", "Step2", "Step3", "Talk", "Defeated1", "Defeated2", "RaiseArms1", "RaiseArms2", "RaiseArms3", "Midair1", "Midair2", "RaisedArmsTalk", "Crouch", "Hurt1", "Hurt2", "Burned", "Sleep1", "Sleep2", "Sleep3", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4" };
const char* const kNpcLilyRasters[] = { "Happy", "TalkHappy", "Dread1", "Dread2", "Idle", "Talk", "LilyPad", "Petals1", "Petals2", "Petals3", "PetalsGiveItem1", "PetalsGiveItem2", "PetalsGiveItem3", "Plead1", "Plead2", "Plead3", "Plead4", "Dread3" };
const char* const kNpcLuigiRasters[] = { "Neutral", "Blink", "Idle", "Talk", "Letter1", "Letter2", "Letter3", "NeutralLetter", "BlinkLetter", "IdleLetter", "TalkLetter", "OpenLetter1", "OpenLetter2", "ReadNeutral", "StepLetter1", "StepLetter2", "StepLetter3", "ReadLetter1", "ReadLetter2", "NeutralRead", "TalkRead", "NeutralSit", "BlinkSit", "TalkSit", "JumpLetter1", "JumpLetter2", "FallLetter1", "FallLetter2", "Jump1", "Jump2", "Fall1", "Fall2", "ReadBlink", "ReadIdle", "JumpStance1", "JumpStance2", "JumpStanceLetter1", "JumpStanceLetter2", "Land", "LandLetter", "IdleBack1", "IdleBack2", "StepBack1", "StepBack2", "StepBack3" };
const char* const kNpcMBushRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C" };
const char* const kNpcMageJrTroopaRasters[] = { "Idle1", "Idle2", "Step1", "Step2", "Step3", "Talk", "Point1", "Sleep1", "Sleep2", "Sleep3", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "BurnStill", "BurnHurt1", "BurnHurt2", "Hurt1", "Hurt2", "Flail1", "Flail2", "RaiseStaff", "ExtendStaff1", "ExtendStaff2", "Point2", "BurnStaff1", "BurnStaff2" };
const char* const kNpcMagikoopaRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12" };
const char* const kNpcMarshalGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A" };
const char* const kNpcMayorPenguinRasters[] = { "Idle1", "Idle2", "Walk1", "Walk2", "Walk3", "Run1", "Run2", "Run3", "Talk", "Carry1", "Carry2", "Carry3", "CarryTalk1", "CarryTalk2", "PickUp1", "PickUp2", "Give1", "Give2", "Give3", "ReachDown", "Dead", "WakeUp1", "WakeUp2", "GetUp1", "GetUp2", "Twitch" };
const char* const kNpcMayorPenguinWifeRasters[] = { "Idle1", "Talk", "Idle2", "Laugh1", "Laugh2", "Step1", "Step2", "Step3", "Inspect1", "Inspect2", "Panic1", "Panic2", "Panic3", "Horror1", "Horror2", "Point1", "Point2", "Point3", "Surprise1", "Surprise2", "Surprise3", "Surprise4" };
const char* const kNpcMediGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07" };
const char* const kNpcMerlarRasters[] = { "Idle", "Talk", "Gather1", "Gather2", "Gather3", "Gather4", "Release1", "Release2" };
const char* const kNpcMerleRasters[] = { "Idle1", "Idle2", "Idle3", "Talk", "Step1", "Step2", "Step3", "Gather1", "Gather2", "Release1", "Release2" };
const char* const kNpcMerlonRasters[] = { "Neutral", "Idle", "Step1", "Step2", "Step3", "Talk", "HappyStep1", "HappyStep2", "HappyStep3", "Think1", "Gather1", "Gather2", "RaiseArms1", "RaiseArms2", "RaiseArms3", "Think2" };
const char* const kNpcMerlowRasters[] = { "Idle1", "Idle2", "Idle3", "Step1", "Step2", "Talk", "GiveItem1", "GiveItem2", "GiveItem3" };
const char* const kNpcMerluvleeRasters[] = { "Idle1", "Idle2", "Happy", "Step1", "Step2", "Step3", "Talk", "Think1", "Think2", "Gather1", "Gather2", "Gather3", "Gather4", "Release1", "Release2" };
const char* const kNpcMinhTRasters[] = { "Neutral", "Idle1", "Idle2", "Idle3", "Idle4", "Blink", "Talk", "Step1", "Step2", "Step3", "Cheer1", "Cheer2" };
const char* const kNpcMonstarRasters[] = { "OutlineIdle1", "OutlineIdle2", "OutlineGather1", "OutlineGather2", "OutlineRearUp1", "OutlineRearUp2", "BodyIdle1", "BodyIdle2", "BodyGather1", "BodyGather2", "BodyRearUp1", "BodyRearUp2", "BodyFlail1", "BodyFlail2", "FaceNeutral", "FaceHappy", "FaceScared", "FaceGather1", "FaceHurt", "FaceTalk", "FaceHappyTalk", "FaceScaredTalk", "FaceGather2", "FaceRelease1", "FaceHappyRearUp1", "FaceRelease2", "FaceHappyRearUp2", "FaceScaredFlail" };
const char* const kNpcMontyMoleRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24" };
const char* const kNpcMouserRasters[] = { "Step1", "Step2", "Step3", "Idle1", "Idle2", "Idle3", "Blink1", "Blink2" };
const char* const kNpcMoustafaRasters[] = { "Step1", "Step2", "Step3", "Still", "Shout", "Talk", "Blink1", "Blink2", "Toss" };
const char* const kNpcMusicianRasters[] = { "Neutral", "Idle1", "Idle2", "Idle3", "Idle4", "Blink", "Pensive1", "Pensive2", "Pensive3", "Talk", "Write1", "Write2", "ConductSlow1", "ConductSlow2", "ConductSlow3", "ConductFast1", "ConductFast2", "ConductFast3", "GivePaper1", "GivePaper2" };
const char* const kNpcNinjiRasters[] = { "Idle1", "Talk", "Idle2", "Step1", "Step2", "Step3" };
const char* const kNpcNomadimouseRasters[] = { "Idle1", "Idle2", "Idle3", "Sack" };
const char* const kNpcOaklieRasters[] = { "Idle1", "Idle2", "Idle3", "Talk", "Jump1", "Jump2", "Fall", "Land1", "Land2", "TakePass1", "TakePass2", "Read1", "Read2", "Read3", "ReadAloud" };
const char* const kNpcParaJrTroopaRasters[] = { "UpperBodyNeutral", "UpperBodyTalk", "UpperBodyPoint", "Sleep1", "Sleep2", "Sleep3", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "WingsFlap1", "WingsFlap2", "WingsFlap3", "WingsFlap4", "LowerBody1", "LowerBody2", "LowerBodyRaised", "Dive1", "Dive2", "Flail1", "Flail2", "Hurt1", "Hurt2", "BurnStill", "BurnHurt1", "BurnHurt2" };
const char* const kNpcParaTroopaRasters[] = { "LWingFlap1", "RWingFlap1", "LWingFlap2", "RWingFlap2", "LWingFlap3", "RWingFlap3", "LWingFlap4", "RWingFlap4", "WingsRecoil1", "WingsRecoil2", "WingsWindup1", "WingsWindup2", "WingStill", "Shell", "IdleLegsDown", "IdleLegsUp", "Recoil", "Windup", "BurnStill", "HurtStill", "Hurt1", "Hurt2", "BurnHurt1", "BurnHurt2", "Sleep", "Stun1", "Stun2", "Stun3", "Stun4", "EnterShell1", "EnterShell2", "EnterShell3", "LWingDive1", "RWingDive2", "LWingDive2", "WingsBurn1", "WingsBurn2" };
const char* const kNpcParadeBannerRasters[] = { "Step1", "Step2", "Step3", "BannerWave1", "BannerWave2", "BannerNeutral", "BannerWave3", "Idle1", "Idle2" };
const char* const kNpcParadeBurntBowserRasters[] = { "ClownCar", "Hair", "Head", "HeadBlink", "ArmsCrossed", "Body", "Propeller1", "Propeller2", "Propeller3", "Propeller4", "Propeller5", "ShadowIdle1", "ShadowIdle2", "ShadowPropeller1", "ShadowPropeller2", "ShadowPropeller3", "ShadowPropeller4", "ShadowPropeller5" };
const char* const kNpcParadeChickRasters[] = { "Chirp1", "Chirp2", "ChirpFwd1", "ChirpFwd2", "BreakEgg", "BreakEggFwd", "Egg", "EggShatter1", "EggShatter2", "EggShatter3", "EggShatter4", "EggShatter5" };
const char* const kNpcParadeDrummerRasters[] = { "Step1", "Step2", "Step3", "Idle1", "Idle2" };
const char* const kNpcParadeHornRasters[] = { "Step1", "Step2", "Step3", "Idle1", "Idle2" };
const char* const kNpcParadeIceShowRasters[] = { "ViolinPlay1", "ViolinPlay2", "ViolinPlay3", "ViolinPlayUpright1", "ViolinPlayUpright2", "ViolinPlayUpright3", "Skate1", "Skate2", "Skate3", "SkateStand", "DivaIdle1", "DivaIdle2", "DivaIdle3", "DivaIdle4", "DivaIdle5", "DivaBlink", "DivaSing1", "DivaSing2", "DivaSing3", "ShadeDivaIdle1", "ShadeDivaIdle2", "ShadeDivaIdle3", "ShadeDivaIdle4", "ShadeDivaIdle5", "ShadeDivaBlink", "ShadeDivaSing1", "ShadeDivaSing2", "ShadeDivaSing3", "ShadeMayorIdle1", "ShadeMayorIdle2", "ShadeMayorWifeIdle1", "ShadeMayorWifeIdle2", "ShadeMayorStep1", "ShadeMayorStep2", "ShadeMayorStep3", "ShadeMayorWifeStep1", "ShadeMayorWifeStep2", "ShadeMayorWifeStep3" };
const char* const kNpcParadeKoloradoRasters[] = { "Idle1", "Idle2", "Idle3", "IdleFwd1", "IdleFwd2", "IdleFwd3", "WifeIdle1", "WifeIdle2", "WifeIdle3", "WifeIdleFwd1", "WifeIdleFwd2", "WifeIdleFwd3", "Wave1", "Wave2", "WifeWave1", "WifeWave2" };
const char* const kNpcParadeKoopasRasters[] = { "ClownCar", "Propeller1", "Propeller2", "Propeller3", "Propeller4", "Propeller5", "BowserHair", "BowserHead", "BowserArmsCrossed", "BowserBody", "HammerBrosJuggle1", "HammerBrosJuggle2", "HammerBrosJuggle3", "HammerBrosPanic1", "HammerBrosPanic2", "HammerBrosPanic3", "Hammer", "KoopatrolMarch1", "KoopatrolMarch2", "KoopatrolMarch3", "KoopatrolPanic1", "KoopatrolPanic2", "KoopatrolPanic3", "KoopatrolFlagMarch1", "KoopatrolFlagMarch2", "KoopatrolFlagMarch3", "KoopatrolFlagPanic1", "KoopatrolFlagPanic2", "KoopatrolFlagPanic3", "KoopatrolFlagPanic4" };
const char* const kNpcParadeLuigiRasters[] = { "Idle1", "Idle2", "Blink", "BlowWhistle1", "March1", "March2", "March3", "MarchInPlace1", "MarchInPlace2", "MarchInPlace3", "BlowWhistle2", "BatonIdle", "BatonWave1", "BatonWave2", "BatonShake1", "BatonShake2", "BatonShake3", "BatonShake4" };
const char* const kNpcParadeMarioRasters[] = { "Neutral", "Step1", "Step2", "Step3", "Blink", "Surprise", "NeutralBack", "SurpriseBlink", "LookUp", "LookDown", "Wave1", "Wave2", "Wave3", "Wave4", "WaveUp1", "WaveUp2", "WaveUp3", "WaveUp4", "StepBack1", "StepBack2", "StepBack3" };
const char* const kNpcParadePartnerRasters[] = { "Goombario1", "Goombario2", "Kooper1", "Kooper2", "Kooper3", "Bombette1", "Bombette2", "Parakarry1", "Parakarry2", "Parakarry3", "LWingFlap1", "RWingFlap1", "LWingFlap2", "RWingFlap2", "LWingFlap3", "RWingFlap3", "LWingFlap4", "RWingFlap4", "Sushie1", "Sushie2", "Sushie3", "LWingFlap5", "RWingFlap5", "Watt1", "Watt2", "Watt3", "Bow1", "Bow2", "BowFan1", "BowFan2", "BowArm1", "BowArm2", "BowPatch", "LakilesterCloud1", "LakilesterCloud2", "LakilesterCloud3", "Lakilester1", "Lakilester2", "Lakilester3", "BombetteGear" };
const char* const kNpcParadePeachRasters[] = { "Neutral", "RaiseArms1", "RaiseArms2", "Talk1", "Talk2", "Talk3", "RaisedArms", "RaisedArmsBlink1", "RaisedArmsBlink2", "Horror1", "Horror2", "Horror3", "Weep1", "Weep2", "Weep3", "ShadeWave1", "ShadeWave2", "ShadeWave3", "ShadeFaceSide", "ShadeKiss1", "ShadeKiss2", "ShadeSlightBow", "LeanBack", "ShadeNeutral", "ShadeStep1", "ShadeStep2", "ShadeStep3", "ShadeNeutralBack", "ShadeStepBack1", "ShadeStepBack2", "ShadeStepBack3", "ShadeRaiseArms1", "ShadeRaiseArms2", "ShadeTalk1", "ShadeTalk2", "ShadeTalk3", "ShadeBlink1", "ShadeBlink2", "Bow1", "Bow2", "Run1", "Run2", "Run3", "Run4" };
const char* const kNpcParadeShyGuyRasters[] = { "GrooveNeutral", "GrooveLeanLeft", "GroovePivot", "GrooveLeanRight", "StackNeutral", "StackPointUp", "StackHold1", "StackHold2", "StackLand1", "StackLand2", "StackLand3", "StackLand4", "MarshallNeutral", "MarshallBeckon1", "MarshallBeckon2", "MarshallBeckon3", "MarshallBeckon4", "MarshallStep1", "MarshallStep2", "MarshallStep3", "MarshallStep4", "GeneralNeutral", "GeneralPoint1", "GeneralPoint2", "ShadeStep1", "ShadeStep2", "ShadeStep3", "ShadeStep4", "ShadePointUp", "ShadeNeutral" };
const char* const kNpcParadeToadRasters[] = { "HornStep1", "HornStep2", "HornStep3", "DrummerStep1", "DrummerStep2", "DrummerStep3", "BannerStep1", "BannerStep2", "BannerStep3", "BannerWave1", "BannerWave2", "BannerNeutral", "BannerWave3", "ToadStill", "ToadetteStill", "TwirlerStep1", "TwirlerStep2", "TwirlerStep3", "TwirlerJoy", "TwirlerToss1", "TwirlerToss2", "Baton", "HornIdle1", "HornIdle2", "DrummerIdle1", "DrummerIdle2", "BannerIdle1", "BannerIdle2", "TwirlerIdle1", "TwirlerIdle2" };
const char* const kNpcParadeTubbaRasters[] = { "Arm", "Hair", "Foot", "Body", "Eyes", "Mouth" };
const char* const kNpcParadeTwinkRasters[] = { "Idle1", "Idle2" };
const char* const kNpcParadeTwirlerRasters[] = { "Neutral", "Step1", "Step2", "Step3", "Joy", "Toss1", "Toss2", "Baton", "Idle1", "Idle2" };
const char* const kNpcParadeWizardRasters[] = { "MerleGather1", "MerleGather2", "MerleRelease3", "MerleRelease4", "MerleeGather1", "MerleeGather2", "MerleeGather3", "MerleeGather4", "MerleeRelease1", "MerleeRelease2", "MerlonGather1", "MerlonGather2", "MerlonRelease3", "MerlonRelease4", "MerlonRelease5", "MerluvleeGather1", "MerluvleeGather2", "MerluvleeGather3", "MerluvleeGather4", "MerluvleeRelease1", "MerluvleeRelease2", "MerlarGather1", "MerlarGather2", "MerlarGather3", "MerlarGather4", "MerlarRelease1", "MerlarRelease2", "MerlowGather1", "MerlowGather2", "MerlowRelease3", "MerlowRelease4" };
const char* const kNpcParadeYoshiRasters[] = { "IdleBrown1", "IdleBrown2", "BlinkBrown", "BlinkGreen", "BlinkBlue", "BlinkYellow", "IdleGreen1", "IdleBlue1", "IdleYellow1", "IdleGreen2", "IdleBlue2", "IdleYellow2", "IdleChief1", "IdleChief2", "IdleChief3", "IdleBlink1", "IdleBlink2" };
const char* const kNpcParagoombaRasters[] = { "Idle1", "Idle2", "Idle3", "Dive", "HurtStill", "WingsBurn", "WingsFlap1", "WingsFlap2", "WingsFlap3", "WingsDive", "LWing", "RWing", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "EyesClosed", "BurnHurt1", "BurnHurt2", "BurnEyesOpen", "Hurt1", "Hurt2", "Electrocute" };
const char* const kNpcPenguinRasters[] = { "Neutral", "Idle1", "Idle2", "Idle3", "Step1", "Step2", "Step3", "Talk1", "Talk2", "Happy1", "Celebrate", "Happy2", "Idle" };
const char* const kNpcPenguinPatrolRasters[] = { "Idle1", "Idle2", "Run1", "Run2", "Run3", "Walk1", "Walk2", "Walk3", "Talk1", "Talk2", "Inspect1", "Inspect2", "TalkThink1", "TalkThink2" };
const char* const kNpcPetitPiranhaRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D" };
const char* const kNpcPetuniaRasters[] = { "LeavesIdle1", "LeavesIdle2", "LeavesRaised", "LeavesGiveItem1", "LeavesGiveItem2", "LeavesDance1", "LeavesDance2", "LeavesDance3", "LeavesDance4", "LeavesBashful1", "LeavesBashful2", "HappyTalk1", "HappyTalk2", "Bashful", "Blink", "Angry", "AngryTalk", "Horror", "Sad", "Happy" };
const char* const kNpcPokeyRasters[] = { "Head", "Body", "Angry", "Idle1", "Idle2", "Hurt1", "Hurt2", "BurnedHurt1", "BurnedHurt2", "BurnedBody", "BurnedHead", "Sleep" };
const char* const kNpcPosieRasters[] = { "Neutral", "Idle1", "Idle2", "Talk", "TalkAngry1", "TalkAngry2", "Confused1", "Confused2", "GiveItem1", "GiveItem2", "GiveItem3", "Strain1", "Strain2" };
const char* const kNpcPostmasterRasters[] = { "Neutral", "Talk", "GiveItem", "LWingFlap1", "RWingFlap1", "LWingFlap2", "RWingFlap2", "LWingFlap3", "RWingFlap3", "LWingFlap4", "RWingFlap4", "LWingFlap5", "RWingFlap5" };
const char* const kNpcPyroGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A" };
const char* const kNpcRaphaelRavenRasters[] = { "Feathers1", "Feathers2", "Feathers3", "EyesHappy", "EyesAngry", "EyesSquint", "Eyes", "Beak1", "Beak2", "Beak3", "Body1", "Body2", "Body3", "Body4", "Body5", "LFoot1", "RFoot1", "LFoot2", "RFoot2", "LFoot3", "RFoot3", "FeathersAngry1", "FeathersAngry2", "BeakAngry1", "BeakAngry2", "BodyAngry", "AngryEmote", "FuriousEmote", "BodyFurious1", "BodyFurious2", "BeakTalkAngry1", "BeakTalkAngry2", "LFootLand", "RFootLand" };
const char* const kNpcRavenRasters[] = { "Idle1", "Idle2", "Idle3", "Step1", "Step2", "Talk", "Flail1", "Flail2", "Flail3", "Crash1", "Crash2", "Crash3", "Crash4", "IdleBack1", "IdleBack2", "IdleBack3" };
const char* const kNpcRipCheatoRasters[] = { "Idle1", "Idle2", "Talk", "Step1", "Step2", "Step3" };
const char* const kNpcRosieRasters[] = { "HeadBlink", "Head1", "Head2", "VinesPullBack", "VinesExtend1", "VinesExtend2", "VinesGiveItem", "VinesHalfExtend", "VinesTalk", "VinesIdle1", "VinesIdle2" };
const char* const kNpcRowfRasters[] = { "Idle1", "Idle2", "Talk1", "Talk2", "WaveOff1", "WaveOff2", "WaveOff3", "WaveOff4", "Step1", "Step2", "Cheer1", "Cheer2", "Think1", "Think2", "PackedIdle1", "PackedIdle2", "PackedStep1", "PackedStep2", "PackedTalk1", "PackedTalk2", "PackedCheer1", "PackedCheer2" };
const char* const kNpcRuffPuffRasters[] = { "Neutral", "Grin", "Hurt", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "Sleep1", "Sleep2", "Blush", "Talk", "Angry", "AngryTalk", "BurnHurt1", "BurnHurt2", "BurnStill" };
const char* const kNpcRussTRasters[] = { "Neutral", "Idle1", "Idle2", "Idle3", "Idle4", "Blink", "Step1", "Step2", "Step3", "Talk", "Lecture1", "Lecture2", "Think1", "Think2", "AdjustGlasses1", "AdjustGlasses2", "Disappointed1", "Disappointed2", "Panic1", "Panic2", "Panic3" };
const char* const kNpcSentinelRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14" };
const char* const kNpcShiverToadRasters[] = { "Neutral", "Idle1", "Idle2", "Idle3", "Idle4", "Blink1", "Blink2", "Blink3", "Talk", "Step1", "Step2", "Step3" };
const char* const kNpcShyGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28" };
const char* const kNpcShySquadGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25" };
const char* const kNpcShyStackDamageRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03" };
const char* const kNpcShyStackGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10" };
const char* const kNpcShyStackRockRasters[] = { "Raster00" };
const char* const kNpcShyStackUnfoldRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04" };
const char* const kNpcSkyGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F", "Raster30" };
const char* const kNpcSmallPiranhaRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10" };
const char* const kNpcSpearGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F", "Raster30", "Raster31", "Raster32", "Raster33", "Raster34", "Raster35", "Raster36", "Raster37", "Raster38", "Raster39", "Raster3A", "Raster3B", "Raster3C", "Raster3D", "Raster3E", "Raster3F", "Raster40", "Raster41", "Raster42", "Raster43", "Raster44", "Raster45", "Raster46", "Raster47", "Raster48", "Raster49", "Raster4A", "Raster4B" };
const char* const kNpcSpikeTopRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23" };
const char* const kNpcSpikedGoombaRasters[] = { "Step1", "Step2", "Step3", "Hurt1", "Midair", "Idle1", "Idle2", "Idle3", "Sleep", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "BurnEyesClosed", "BurnEyesOpen", "Laugh1", "Laugh2", "Hurt2", "Electrocute" };
const char* const kNpcSpikedJrTroopaRasters[] = { "Idle1", "Idle2", "Step1", "Step2", "Step3", "Sleep1", "Sleep2", "Sleep3", "Jump1", "Jump2", "Midair", "Fall", "Land1", "Land2", "Hurt1", "Hurt2", "BurnStill", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "Talk", "PointNeutral", "Angry1", "Angry2", "Angry3", "BurnHurt1", "BurnHurt2", "PointIdle" };
const char* const kNpcSpikedParaJrTroopaRasters[] = { "UpperBodyNeutral", "UpperBodyTalk", "UpperBodyPoint", "Sleep1", "Sleep2", "Sleep3", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "WingsFlap1", "WingsFlap2", "WingsFlap3", "WingsFlap4", "LowerBody1", "LowerBody2", "LowerBodyRaised", "Dive1", "Dive2", "Flail1", "Flail2", "Hurt1", "Hurt2", "BurnStill", "BurnHurt1", "BurnHurt2", "EnterEgg", "EggNeutral", "BurnWingsFlap1", "BurnWingsFlap2", "BurnWingsFlap3", "BurnWingsFlap4", "BurnEgg", "Angry1", "Angry2" };
const char* const kNpcSpikyGoomnutRasters[] = { "Still" };
const char* const kNpcSpinyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E" };
const char* const kNpcSpyGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24" };
const char* const kNpcStarManRasters[] = { "Idle1", "Idle2", "Talk" };
const char* const kNpcStarRodRasters[] = { "Still" };
const char* const kNpcStiltGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12" };
const char* const kNpcStiltGuyUnfoldRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04" };
const char* const kNpcStoneChompRasters[] = { "Neutral", "Chain", "Bite", "Hurt1", "Hurt2", "BurnHurt1", "BurnHurt2", "BurnChain", "Sleep" };
const char* const kNpcSunRasters[] = { "EyesClosed", "Blink1", "Blink2", "TalkSad1", "TalkSad2", "TalkJoy1", "TalkJoy2", "TalkJoy3", "TalkHappy1", "TalkHappy2", "TalkHappy3", "MajorFlame1", "MajorFlame2", "MinorFlame1", "MinorFlame2" };
const char* const kNpcSwooperRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A" };
const char* const kNpcTankGuyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12" };
const char* const kNpcTayceTRasters[] = { "Idle1", "Idle2", "Step1", "Step2", "Step3", "Talk1", "Talk2", "Carry1", "Carry2", "Carry3", "Happy1", "Happy2", "Sad1", "Sad2", "IdleFwd", "IdleBack1", "IdleBack2", "IdleBack3", "TurnAround1", "TalkFwd", "TurnAround2", "Pan" };
const char* const kNpcTheMasterRasters[] = { "Neutral", "Idle", "Blink", "Cough1", "Cough2", "Step1", "Step2", "Step3", "Step4", "Talk", "Beckon1", "Beckon2", "Beckon3", "OpenMouth", "AscendedNeutral", "AscendedDownwardStance1", "AscendedDownwardStance2", "AscendedDefeated1", "AscendedDefeated2", "AscendeHurt1", "AscendeHurt2", "BurnStill", "AscendedGuard1", "AscendedGuard2", "AscendedGuard3", "AscendedGuard4", "AscendedUppercut1", "AscendedUppercut2", "AscendedUppercut3", "AscendedUppercut4", "AscendedStep1", "AscendedStep2", "AscendedStep3", "AscendedStep4", "AscendedIdle1", "AscendedIdle2", "AscendedIdle3", "AscendedTalk", "AscendedPunch1", "AscendedPunch2", "AscendedPunch3", "AscendedPunch4", "AscendedPunch5", "AscendedPunch6", "AscendedKick1", "AscendedKick2", "AscendedKick3", "AscendedPalmStance1", "AscendedPalmStance2", "AscendedPalmStance3", "AscendedPalmStance4", "BurnHurt1", "BurnHurt2", "Sleep1", "Sleep2", "Sleep3", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "AscendedSleep1", "AscendedSleep2", "AscendedSleep3", "AscendedDizzy1", "AscendedDizzy2", "AscendedDizzy3", "AscendedDizzy4", "Defeated1", "Defeated2", "Hurt1", "Hurt2", "AscendedCrouch", "AscendedMidair", "AscendedDive", "Windup1", "Windup2", "DownwardStrike1", "DownwardStrike2" };
const char* const kNpcThrownBoneRasters[] = { "Still" };
const char* const kNpcToadRasters[] = { "Neutral", "Idle1", "Idle2", "Idle3", "Idle4", "Blink", "Step1", "Step2", "Step3", "Talk", "Disappointed1", "Disappointed2", "Disappointed3", "Celebrate1", "Celebrate2", "GiveItem", "Panic1", "Panic2", "Panic3" };
const char* const kNpcToadGuardRasters[] = { "Neutral", "Idle1", "Idle2", "Talk", "Step1", "Step2", "Step3" };
const char* const kNpcToadKidRasters[] = { "Neutral", "Idle", "Step1", "Step2", "Step3", "Talk", "Disappointed1", "Disappointed2", "Celebrate1", "Celebrate2", "Celebrate3" };
const char* const kNpcToadMinisterRasters[] = { "Neutral", "Idle1", "Idle2", "Step1", "Step2", "Step3", "Think1", "Think2", "Talk", "RaiseArms", "Invite1", "Invite2" };
const char* const kNpcToadetteRasters[] = { "Neutral", "Idle1", "Idle2", "Disappointed1", "Idle3", "Idle4", "Blink", "Disappointed2", "Step1", "Step2", "Step3", "GiveItem", "Celebrate1", "Celebrate2", "Talk" };
const char* const kNpcTolielupRasters[] = { "MouthNeutral", "MouthTalk1", "MouthTalk2", "MouthTalk3", "MouthLaugh1", "MouthLaugh2", "Flower", "Stem1", "Stem2", "LLeaf1", "LLeaf2", "LLeaf3", "RLeaf1", "RLeaf2", "RLeaf3", "LEyeIdle1", "REyeIdle1", "LEyeLaugh1", "REyeLaugh1", "LEyeTalk1", "REyeTalk1", "LEyeLaugh2", "REyeLaugh2", "LEyeIdle2", "REyeIdle2", "LEyeTalk2", "REyeTalk2" };
const char* const kNpcTrainToadRasters[] = { "Neutral", "Idle1", "Idle2", "Idle3", "Idle4", "Blink", "Talk", "Sadden", "SadNeutral", "SadTalk", "SadIdle", "Step1", "Step2", "Step3" };
const char* const kNpcTubbasHeartRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B" };
const char* const kNpcTuffPuffRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D" };
const char* const kNpcTutankoopaRasters[] = { "Headdress", "HeaddressWithEyes", "NoHeaddress", "Neutral", "Talk", "Run1", "Run2", "Run3", "Shout1", "Shout2", "Levitate1", "Levitate2", "Toss1", "Toss2", "Fall1", "Fall2", "Hurt1", "Hurt2", "Dizzy1", "Dizzy2", "Dizzy3", "Dizzy4", "Lift", "Idle", "BurnHurt1", "BurnHurt2", "BurnHurt3", "BurnFall1", "BurnFall2", "Rock", "RockShard1", "RockShard2", "RockShard3", "RockShard4", "IdleChompBite1", "IdleChompBite2", "Sleep1", "Sleep2", "SleepGround1", "SleepGround2", "SleepGround3", "Glare1", "Glare2", "Glare3", "Glare4", "Glare5", "Glare6", "Glare7" };
const char* const kNpcTwinkRasters[] = { "Idle1", "Idle2", "IdleAngry1", "IdleAngry2", "IdleAngry3", "Talk", "Shout1", "Shout2", "Cringe1", "Cringe2", "Cringe3", "TalkSad1", "TalkSad2", "ShoutJoy1", "ShoutJoy2", "Carry1", "Carry2", "Carry3", "Tense1", "Tense2", "Wink1", "Wink2", "TossItem", "Pleased1", "Pleased2", "Pleased3", "Disappointed1", "Disappointed2", "Back1", "Back2", "AngryAlt1", "AngryAlt2", "IdleSad1", "IdleSad2", "TalkAngry", "IdleLean1", "IdleLean2", "TalkLean", "LiftBook", "OpenBook1", "OpenBook2", "BookLifted", "BookHeld", "BookHalfOpen", "BookOpen", "BookClosed", "ShoutAngry", "Hurt1", "Hurt2" };
const char* const kNpcVannaTRasters[] = { "Neutral", "Step1", "Step2", "Step3", "Happy1", "Happy2", "Clap1", "Clap2", "Shrug1", "Shrug2", "Wave1", "Wave2" };
const char* const kNpcVillageLeaderRasters[] = { "Idle1", "Idle2", "Step1", "Step2", "Step3", "Talk", "CrossArms1", "CrossArms2", "Celebrate1", "Celebrate2", "Shout1", "Fall", "Land", "IdleSit1", "IdleSit2", "IdleSit3", "IdleSad1", "IdleSad2", "Shout2", "Panic1", "Panic2", "Panic3", "Rummage1", "Rummage2", "TalkSit" };
const char* const kNpcWhackaRasters[] = { "Neutral", "Popup1", "Popup2", "Popup3", "Idle", "Talk", "Hurt1", "Hurt2", "Hurt3", "Hurt4", "Hurt5", "Hurt6", "Hurt7", "Hurt8", "HoleOpening1", "HoleOpening2", "HoleOpen" };
const char* const kNpcWorldBobombRasters[] = { "Step1", "Step2", "Step3", "Idle", "Talk", "Neutral", "GearNeutral", "GearSpin1", "GearSpin2", "GearSpin3", "GearSpin4", "GearSpin5" };
const char* const kNpcWorldBombetteRasters[] = { "Neutral", "Step1", "Step2", "Step3", "Step4", "Idle1", "Idle2", "Idle3", "Idle4", "Jump", "Fall", "GearAngled", "LitStill", "LitStep1", "LitStep2", "LitStep3", "AboutToExplode", "Hurt1", "Hurt2", "Talk", "IdleHappy1", "IdleHappy2", "IdleHappy3", "IdleHappy4", "IdleHappy5", "JumpHappy", "FallHappy", "FuseLit1", "FuseLit2", "FuseLit3", "FuseEnd1", "FuseEnd2", "FuseEnd3", "GearNeutral", "GearSpin1", "GearSpin2", "GearSpin3", "GearSpin4", "GearSpin5", "GearSpin6", "GearSpin7" };
const char* const kNpcWorldBowRasters[] = { "Neutral", "Step1", "Step2", "Talk1", "Talk2", "Conceal", "Spook1", "Spook2", "Spook3", "Spook4", "Laugh1", "Laugh2", "Celebrate1", "Celebrate2", "Celebrate3", "Celebrate4", "Flail1", "Flail2", "TailDown", "Read", "FanAcrossBody1", "FanAcrossBody2", "ArmHoldingFan1", "ArmHoldingFan2", "SmallPatch" };
const char* const kNpcWorldBowserRasters[] = { "Head", "BodyWalk", "Shell", "LArm", "RArm", "HeadAngry", "LFoot", "RFoot", "LFootStep1", "RFootStep1", "LFootStep2", "RFootStep2", "LFootStep3", "RFootStep3", "HeadTalk1", "HeadTalk2", "HeadEyesClosed1", "HeadEyesClosed2", "HeadMock3", "BodyStand", "BodyRotated", "LFootRotated", "RFootRotated", "FootBottom1", "LArmBrandish", "RArmBrandish", "RHandBrandish", "LArmRaised", "RArmRaised1", "RArmRaised2", "LArmCrossed", "RArmCrossed", "LArmBent", "RArmBent", "BodyLand", "BodyInsideCar", "ClownCar", "HeadFwdIdle1", "HeadFwdIdle2", "HeadFwdOpenMouth", "HeadFwdBrandish1", "HeadFwdBrandish2", "StarRod", "StarRodTip", "StarRodShaft", "HeadFwdTalk1", "HeadFwdTalk2", "HeadFwdFireBreath", "LArmClownCar", "RArmClownCar", "Propeller1", "Propeller2", "Propeller3", "Propeller4", "Propeller5", "DefeatedHead", "DefeatedHeadBlink", "DefeatedHeadTalk1", "DefeatedHeadTalk2", "DefeatedBody", "DefeatedRArm", "DefeatedRFoot", "FootBottom2", "LFist", "LArmFist", "RArmFist", "RFist", "HeadFwdLaugh1", "HeadFwdLaugh2", "HeadRotated1", "HeadRotated2", "HeadRotated3", "HeadTantrum", "HeadSurprise", "HeadEyesClosed", "HeadLaugh1", "HeadLaugh2", "LArmSwipe", "RFistSwipe", "RArmSwipe", "BodyFwd", "LFootTantrum", "RFootTantrum" };
const char* const kNpcWorldClubbaRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F" };
const char* const kNpcWorldEldstarRasters[] = { "Idle1", "Idle2", "Wave1", "Wave2", "Leap1", "Leap2", "Panic", "Angry1", "Angry2", "Hurt1", "Hurt2", "Back1", "Back2", "Talk" };
const char* const kNpcWorldGoombarioRasters[] = { "Step1", "Step2", "Step3", "Hurt", "Headbonk", "Idle1", "Idle2", "Idle3", "Talk1", "Talk2", "Jump", "Fall", "Happy1", "Happy2", "Read" };
const char* const kNpcWorldKalmarRasters[] = { "Idle1", "Idle2", "Talk", "Leap1", "Leap2", "TalkHappy1", "TalkHappy2", "Panic1", "Panic2", "Back1", "Back2", "Hurt1", "Hurt2" };
const char* const kNpcWorldKammyRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C" };
const char* const kNpcWorldKlevarRasters[] = { "Idle1", "Idle2", "Talk", "Leap1", "Leap2", "Panic1", "Panic2", "TalkHappy1", "TalkHappy2", "Back1", "Back2", "Hurt1", "Hurt2", "IdleHappy1", "IdleHappy2" };
const char* const kNpcWorldKoopatrolRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E" };
const char* const kNpcWorldKooperRasters[] = { "Step1", "Step2", "Step3", "Idle1", "Idle2", "Idle3", "Idle4", "Shell1", "Shell2", "Shell3", "Shell4", "Shell5", "Shell6", "Shell7", "Shell8", "Hurt1", "Hurt2", "EnterShell1", "EnterShell2", "EnterShell3", "Talk", "TalkHappy1", "TalkHappy2", "Pleased", "Celebrate1", "Celebrate2", "Celebrate3", "Jump", "Fall", "Read", "TalkSad1", "TalkSad2" };
const char* const kNpcWorldLakilesterRasters[] = { "CloudIdle1", "CloudIdle2", "CloudIdle3", "Vanish1", "Vanish2", "Vanish3", "Vanish4", "Vanish5", "Vanish6", "Vanish7", "Vanish8", "CloudStrain1", "CloudStrain2", "CloudStrain3", "CloudTough1", "CloudTough2", "CloudHappy", "TiredTalk", "Idle", "Step1", "Step2", "Step3", "Talk", "Tough", "Pant1", "Pant2", "Strain1", "Strain2", "Strain3", "Shout1", "Shout2", "Happy1", "Happy2", "Tired", "CloudTired1", "CloudTired2" };
const char* const kNpcWorldMamarRasters[] = { "Idle1", "Idle2", "Back1", "Back2", "TalkHappy", "Panic", "Angry1", "Angry2", "Leap1", "Leap2", "Talk", "Hurt1", "Hurt2" };
const char* const kNpcWorldMerleeRasters[] = { "Idle1", "Idle2", "Idle3", "Step1", "Step2", "Talk", "Release1", "Release2", "Kneel", "SpinArmsUp", "SpinFront", "SpinBack", "Bow", "Gather1", "Gather2", "Gather3", "Gather4" };
const char* const kNpcWorldMisstarRasters[] = { "Idle1", "Idle2", "Talk", "Leap1", "Leap2", "Happy1", "Happy2", "Panic1", "Panic2", "IdleAngry1", "IdleAngry2", "TalkAngry", "Back1", "Back2", "Hurt1", "Hurt2", "Fly" };
const char* const kNpcWorldMuskularRasters[] = { "Idle1", "Idle2", "Talk", "Happy1", "Happy2", "Leap1", "Leap2", "Panic1", "Panic2", "Hurt1", "Hurt2", "Back1", "Back2" };
const char* const kNpcWorldParakarryRasters[] = { "LWingFlap1", "LWingFlap2", "LWingFlap3", "LWingFlap4", "LegsBack", "LegsForward", "Talk", "Celebrate1", "Celebrate2", "Hurt1", "Hurt2", "Lift1", "Lift2", "RWingFlap1", "RWingFlap2", "RWingFlap3", "RWingFlap4", "RWingFlap5", "LWingFlap5", "Blink1", "Blink2", "RaisedArm" };
const char* const kNpcWorldSkolarRasters[] = { "Idle1", "Idle2", "Talk1", "Talk2", "Leap1", "Leap2", "Hurt1", "Hurt2", "Back1", "Back2", "IdleSad1", "IdleSad2", "TalkSad", "Panic" };
const char* const kNpcWorldSushieRasters[] = { "Idle1", "Idle2", "Idle3", "Talk1", "Talk2", "HappyTalk1", "HappyTalk2", "Sad1", "Sad2", "Hurt1", "Hurt2", "Celebrate1", "Celebrate2", "Ride1", "Ride2", "Dive1", "Dive2", "Rise1", "Rise2", "Read" };
const char* const kNpcWorldTubbaRasters[] = { "Raster00", "Raster01", "Raster02", "Raster03", "Raster04", "Raster05", "Raster06", "Raster07", "Raster08", "Raster09", "Raster0A", "Raster0B", "Raster0C", "Raster0D", "Raster0E", "Raster0F", "Raster10", "Raster11", "Raster12", "Raster13", "Raster14", "Raster15", "Raster16", "Raster17", "Raster18", "Raster19", "Raster1A", "Raster1B", "Raster1C", "Raster1D", "Raster1E", "Raster1F", "Raster20", "Raster21", "Raster22", "Raster23", "Raster24", "Raster25", "Raster26", "Raster27", "Raster28", "Raster29", "Raster2A", "Raster2B", "Raster2C", "Raster2D", "Raster2E", "Raster2F", "Raster30", "Raster31", "Raster32", "Raster33", "Raster34", "Raster35", "Raster36", "Raster37", "Raster38", "Raster39", "Raster3A", "Raster3B", "Raster3C", "Raster3D", "Raster3E", "Raster3F", "Raster40", "Raster41", "Raster42", "Raster43", "Raster44", "Raster45", "Raster46", "Raster47", "Raster48", "Raster49", "Raster4A", "Raster4B", "Raster4C", "Raster4D", "Raster4E", "Raster4F" };
const char* const kNpcWorldWattRasters[] = { "Idle1", "Idle2", "Idle3", "Talk", "Happy1", "Happy2", "Happy3", "Hurt1", "Hurt2", "Hurt3", "Strain1", "Strain2", "Strain3", "ULBolt", "LLBolt", "URBolt", "LRBolt" };
const char* const kNpcYakkeyRasters[] = { "Idle1", "Idle2", "Idle3", "Talk1", "Talk2", "Talk3", "Use1", "Use2", "Use3", "Use4", "Use5", "Use6" };
const char* const kNpcYoshiRasters[] = { "Idle1", "Idle2", "Step1", "Step2", "Step3", "Talk", "Celebrate1", "Celebrate2", "Disappointed1", "Disappointed2", "Panic1", "Panic2", "Panic3", "Cry1", "Cry2", "Cry3" };
const char* const kNpcYoshiKidRasters[] = { "Idle1", "Idle2", "Talk", "Step1", "Step2", "Step3", "Land", "Jump", "Sleep1", "Sleep2", "Sleep3", "Cry1", "Cry2", "Cry3", "Cry4", "SadIdle1", "SadIdle2", "SadTalk", "LookUp", "Exhale1", "Exhale2", "Exhale3", "Eat1", "Eat2", "Eat3", "Eat4", "Tongue1", "Tongue2", "Food1", "Food2", "Food3", "Food4", "Food5", "Food6", "Tear1", "Tear2", "Tear3" };
const char* const kPlayerMario1Rasters[] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F", "50", "51", "52", "53", "54", "55", "56" };
const char* const kPlayerMarioB1Rasters[] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F", "40" };
const char* const kPlayerMarioB2Rasters[] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D" };
const char* const kPlayerMarioB3Rasters[] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E" };
const char* const kPlayerMarioW1Rasters[] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F", "30", "31", "32", "33", "34", "35", "36", "37", "38" };
const char* const kPlayerMarioW2Rasters[] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F" };
const char* const kPlayerMarioW3Rasters[] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C" };
const char* const kPlayerPeach1Rasters[] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D" };
const char* const kPlayerPeach2Rasters[] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B", "2C", "2D", "2E", "2F", "30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B", "3C", "3D", "3E", "3F", "40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F", "50", "51", "52", "53", "54", "55", "56", "57", "58" };
const char* const kPlayerPeach3Rasters[] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B", "0C", "0D", "0E", "0F", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D", "1E", "1F", "20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2A", "2B" };
} // namespace

const SpriteSheetName kNpcSpriteNames[] = {
    { "Goombario", "World Goombario", kNpcWorldGoombarioRasters, 15 },
    { "Kooper", "World Kooper", kNpcWorldKooperRasters, 32 },
    { "Bombette", "World Bombette", kNpcWorldBombetteRasters, 41 },
    { "Parakarry", "World Parakarry", kNpcWorldParakarryRasters, 22 },
    { "Bow", "World Bow", kNpcWorldBowRasters, 25 },
    { "Watt", "World Watt", kNpcWorldWattRasters, 17 },
    { "Sushie", "World Sushie", kNpcWorldSushieRasters, 20 },
    { "Lakilester", "World Lakilester", kNpcWorldLakilesterRasters, 36 },
    { "Goombario", "Battle Goombario", kNpcBattleGoombarioRasters, 25 },
    { "Kooper", "Battle Kooper", kNpcBattleKooperRasters, 38 },
    { "Bombette", "Battle Bombette", kNpcBattleBombetteRasters, 50 },
    { "Parakarry", "Battle Parakarry", kNpcBattleParakarryRasters, 35 },
    { "Bow", "Battle Bow", kNpcBattleBowRasters, 43 },
    { "Watt", "Battle Watt", kNpcBattleWattRasters, 36 },
    { "Sushie", "Battle Sushie", kNpcBattleSushieRasters, 29 },
    { "Lakilester", "Battle Lakilester", kNpcBattleLakilesterRasters, 45 },
    { "Kooper Without Shell", "Kooper Without Shell", kNpcKooperWithoutShellRasters, 25 },
    { "Eldstar", "World Eldstar", kNpcWorldEldstarRasters, 14 },
    { "Mamar", "World Mamar", kNpcWorldMamarRasters, 13 },
    { "Skolar", "World Skolar", kNpcWorldSkolarRasters, 14 },
    { "Muskular", "World Muskular", kNpcWorldMuskularRasters, 13 },
    { "Misstar", "World Misstar", kNpcWorldMisstarRasters, 17 },
    { "Klevar", "World Klevar", kNpcWorldKlevarRasters, 15 },
    { "Kalmar", "World Kalmar", kNpcWorldKalmarRasters, 13 },
    { "Eldstar", "Battle Eldstar", kNpcBattleEldstarRasters, 3 },
    { "Mamar", "Battle Mamar", kNpcBattleMamarRasters, 3 },
    { "Skolar", "Battle Skolar", kNpcBattleSkolarRasters, 3 },
    { "Muskular", "Battle Muskular", kNpcBattleMuskularRasters, 3 },
    { "Misstar", "Battle Misstar", kNpcBattleMisstarRasters, 3 },
    { "Klevar", "Battle Klevar", kNpcBattleKlevarRasters, 3 },
    { "Kalmar", "Battle Kalmar", kNpcBattleKalmarRasters, 3 },
    { "Twink", "Twink", kNpcTwinkRasters, 49 },
    { "Jr Troopa", "Jr Troopa", kNpcJrTroopaRasters, 75 },
    { "Spiked Jr Troopa", "Spiked Jr Troopa", kNpcSpikedJrTroopaRasters, 29 },
    { "Spiked Para Jr Troopa", "Spiked Para Jr Troopa", kNpcSpikedParaJrTroopaRasters, 35 },
    { "Mage Jr Troopa", "Mage Jr Troopa", kNpcMageJrTroopaRasters, 27 },
    { "Para Jr Troopa", "Para Jr Troopa", kNpcParaJrTroopaRasters, 26 },
    { "Goomba", "Goomba", kNpcGoombaRasters, 20 },
    { "Spiked Goomba", "Spiked Goomba", kNpcSpikedGoombaRasters, 19 },
    { "Paragoomba", "Paragoomba", kNpcParagoombaRasters, 23 },
    { "Koopa Troopa", "Koopa Troopa", kNpcKoopaTroopaRasters, 42 },
    { "Para Troopa", "Para Troopa", kNpcParaTroopaRasters, 37 },
    { "Fuzzy", "Fuzzy", kNpcFuzzyRasters, 24 },
    { "Bobomb", "Bobomb", kNpcBobombRasters, 50 },
    { "Bullet Bill", "Bullet Bill", kNpcBulletBillRasters, 12 },
    { "Bill Blaster", "Bill Blaster", kNpcBillBlasterRasters, 10 },
    { "Monty Mole", "Monty Mole", kNpcMontyMoleRasters, 37 },
    { "Cleft", "Cleft", kNpcCleftRasters, 40 },
    { "Pokey", "Pokey", kNpcPokeyRasters, 12 },
    { "Bandit", "Bandit", kNpcBanditRasters, 45 },
    { "Buzzy Beetle", "Buzzy Beetle", kNpcBuzzyBeetleRasters, 51 },
    { "Swooper", "Swooper", kNpcSwooperRasters, 43 },
    { "Stone Chomp", "Stone Chomp", kNpcStoneChompRasters, 9 },
    { "Large Piranha", "Large Piranha", kNpcLargePiranhaRasters, 37 },
    { "Small Piranha", "Small Piranha", kNpcSmallPiranhaRasters, 17 },
    { "Sentinel", "Sentinel", kNpcSentinelRasters, 21 },
    { "Clubba", "World Clubba", kNpcWorldClubbaRasters, 48 },
    { "Clubba", "Battle Clubba", kNpcBattleClubbaRasters, 34 },
    { "Shy Guy", "Shy Guy", kNpcShyGuyRasters, 41 },
    { "Groove Guy", "Groove Guy", kNpcGrooveGuyRasters, 26 },
    { "Sky Guy", "Sky Guy", kNpcSkyGuyRasters, 49 },
    { "Pyro Guy", "Pyro Guy", kNpcPyroGuyRasters, 27 },
    { "Spy Guy", "Spy Guy", kNpcSpyGuyRasters, 37 },
    { "Medi Guy", "Medi Guy", kNpcMediGuyRasters, 8 },
    { "Fuzzipede", "Fuzzipede", kNpcFuzzipedeRasters, 42 },
    { "Spear Guy", "Spear Guy", kNpcSpearGuyRasters, 76 },
    { "Heart Plant", "Heart Plant", kNpcHeartPlantRasters, 20 },
    { "Hurt Plant", "Hurt Plant", kNpcHurtPlantRasters, 57 },
    { "M Bush", "M Bush", kNpcMBushRasters, 29 },
    { "Lava Bubble", "Lava Bubble", kNpcLavaBubbleRasters, 28 },
    { "Kent C Koopa", "Kent C Koopa", kNpcKentCKoopaRasters, 94 },
    { "Dayzee", "Dayzee", kNpcDayzeeRasters, 29 },
    { "Lakitu", "Lakitu", kNpcLakituRasters, 52 },
    { "Spiny", "Spiny", kNpcSpinyRasters, 47 },
    { "Bzzap", "Bzzap", kNpcBzzapRasters, 38 },
    { "Tuff Puff", "Tuff Puff", kNpcTuffPuffRasters, 14 },
    { "Spike Top", "Spike Top", kNpcSpikeTopRasters, 36 },
    { "Duplighost", "Duplighost", kNpcDuplighostRasters, 28 },
    { "Albino Dino", "Albino Dino", kNpcAlbinoDinoRasters, 22 },
    { "Blooper", "Blooper", kNpcBlooperRasters, 20 },
    { "Baby Blooper", "Baby Blooper", kNpcBabyBlooperRasters, 19 },
    { "Gulpit", "Gulpit", kNpcGulpitRasters, 30 },
    { "Dry Bones", "Dry Bones", kNpcDryBonesRasters, 36 },
    { "Thrown Bone", "Thrown Bone", kNpcThrownBoneRasters, 1 },
    { "Bony Beetle", "Bony Beetle", kNpcBonyBeetleRasters, 81 },
    { "Magikoopa", "Magikoopa", kNpcMagikoopaRasters, 19 },
    { "Flying Magikoopa", "Flying Magikoopa", kNpcFlyingMagikoopaRasters, 23 },
    { "Koopatrol", "World Koopatrol", kNpcWorldKoopatrolRasters, 47 },
    { "Koopatrol", "Koopatrol", kNpcKoopatrolRasters, 42 },
    { "Hammer Bros", "Hammer Bros", kNpcHammerBrosRasters, 41 },
    { "Bush Basic", "Bush Basic", kNpcBushBasicRasters, 1 },
    { "Bush Blocky", "Bush Blocky", kNpcBushBlockyRasters, 1 },
    { "Bush Dry", "Bush Dry", kNpcBushDryRasters, 1 },
    { "Bush Leafy", "Bush Leafy", kNpcBushLeafyRasters, 1 },
    { "Bush Matted", "Bush Matted", kNpcBushMattedRasters, 1 },
    { "Kammy", "World Kammy", kNpcWorldKammyRasters, 45 },
    { "Kammy", "Battle Kammy", kNpcBattleKammyRasters, 32 },
    { "Goomba Bros", "Goomba Bros", kNpcGoombaBrosRasters, 31 },
    { "Goomba King", "Goomba King", kNpcGoombaKingRasters, 37 },
    { "Spiky Goomnut", "Spiky Goomnut", kNpcSpikyGoomnutRasters, 1 },
    { "Dark Toad", "Dark Toad", kNpcDarkToadRasters, 13 },
    { "Koopa Bros", "Koopa Bros", kNpcKoopaBrosRasters, 49 },
    { "Buzzar", "Buzzar", kNpcBuzzarRasters, 82 },
    { "Tutankoopa", "Tutankoopa", kNpcTutankoopaRasters, 48 },
    { "Chain Chomp", "Chain Chomp", kNpcChainChompRasters, 6 },
    { "Tubba", "World Tubba", kNpcWorldTubbaRasters, 80 },
    { "Tubba", "Battle Tubba", kNpcBattleTubbaRasters, 74 },
    { "Tubbas Heart", "Tubbas Heart", kNpcTubbasHeartRasters, 28 },
    { "Big Lantern Ghost", "Big Lantern Ghost", kNpcBigLanternGhostRasters, 38 },
    { "Shy Squad Guy", "Shy Squad Guy", kNpcShySquadGuyRasters, 38 },
    { "Marshal Guy", "Marshal Guy", kNpcMarshalGuyRasters, 27 },
    { "Stilt Guy", "Stilt Guy", kNpcStiltGuyRasters, 19 },
    { "Stilt Guy Unfold", "Stilt Guy Unfold", kNpcStiltGuyUnfoldRasters, 5 },
    { "Shy Stack Guy", "Shy Stack Guy", kNpcShyStackGuyRasters, 17 },
    { "Shy Stack Unfold", "Shy Stack Unfold", kNpcShyStackUnfoldRasters, 5 },
    { "Shy Stack Damage", "Shy Stack Damage", kNpcShyStackDamageRasters, 4 },
    { "Shy Stack Rock", "Shy Stack Rock", kNpcShyStackRockRasters, 1 },
    { "General Guy", "General Guy", kNpcGeneralGuyRasters, 27 },
    { "General Guy Bomb", "General Guy Bomb", kNpcGeneralGuyBombRasters, 5 },
    { "Tank Guy", "Tank Guy", kNpcTankGuyRasters, 19 },
    { "Lava Piranha", "Lava Piranha", kNpcLavaPiranhaRasters, 23 },
    { "Petit Piranha", "Petit Piranha", kNpcPetitPiranhaRasters, 14 },
    { "Lava Bud", "Lava Bud", kNpcLavaBudRasters, 29 },
    { "Huff N Puff", "Huff N Puff", kNpcHuffNPuffRasters, 56 },
    { "Ruff Puff", "Ruff Puff", kNpcRuffPuffRasters, 16 },
    { "Monstar", "Monstar", kNpcMonstarRasters, 28 },
    { "Crystal King", "Crystal King", kNpcCrystalKingRasters, 56 },
    { "Bowser", "World Bowser", kNpcWorldBowserRasters, 83 },
    { "Bowser", "Battle Bowser", kNpcBattleBowserRasters, 69 },
    { "Luigi", "Luigi", kNpcLuigiRasters, 45 },
    { "Toad", "Toad", kNpcToadRasters, 19 },
    { "Toadette", "Toadette", kNpcToadetteRasters, 15 },
    { "Vanna T", "Vanna T", kNpcVannaTRasters, 12 },
    { "Toad Kid", "Toad Kid", kNpcToadKidRasters, 11 },
    { "Toad Guard", "Toad Guard", kNpcToadGuardRasters, 7 },
    { "Harry T", "Harry T", kNpcHarryTRasters, 10 },
    { "Toad Minister", "Toad Minister", kNpcToadMinisterRasters, 12 },
    { "Postmaster", "Postmaster", kNpcPostmasterRasters, 13 },
    { "Conductor Toad", "Conductor Toad", kNpcConductorToadRasters, 11 },
    { "Train Toad", "Train Toad", kNpcTrainToadRasters, 14 },
    { "Fishmael", "Fishmael", kNpcFishmaelRasters, 8 },
    { "Musician", "Musician", kNpcMusicianRasters, 20 },
    { "Koopa", "Koopa", kNpcKoopaRasters, 19 },
    { "Koopa Without Shell", "Koopa Without Shell", kNpcKoopaWithoutShellRasters, 28 },
    { "Bobomb", "World Bobomb", kNpcWorldBobombRasters, 12 },
    { "Whacka", "Whacka", kNpcWhackaRasters, 17 },
    { "Dryite", "Dryite", kNpcDryiteRasters, 5 },
    { "Mouser", "Mouser", kNpcMouserRasters, 8 },
    { "Boo", "Boo", kNpcBooRasters, 33 },
    { "Yoshi", "Yoshi", kNpcYoshiRasters, 16 },
    { "Yoshi Kid", "Yoshi Kid", kNpcYoshiKidRasters, 37 },
    { "Raven", "Raven", kNpcRavenRasters, 16 },
    { "Bubulb", "Bubulb", kNpcBubulbRasters, 38 },
    { "Penguin", "Penguin", kNpcPenguinRasters, 13 },
    { "Shiver Toad", "Shiver Toad", kNpcShiverToadRasters, 12 },
    { "Alt Bandit", "Alt Bandit", kNpcAltBanditRasters, 27 },
    { "Goompa", "Goompa", kNpcGoompaRasters, 26 },
    { "Goombaria", "Goombaria", kNpcGoombariaRasters, 36 },
    { "Gooma", "Gooma", kNpcGoomaRasters, 6 },
    { "Goompapa", "Goompapa", kNpcGoompapaRasters, 18 },
    { "Goomama", "Goomama", kNpcGoomamaRasters, 8 },
    { "The Master", "The Master", kNpcTheMasterRasters, 78 },
    { "Chan", "Chan", kNpcChanRasters, 30 },
    { "Lee", "Lee", kNpcLeeRasters, 27 },
    { "Merlon", "Merlon", kNpcMerlonRasters, 16 },
    { "Chet Rippo", "Chet Rippo", kNpcChetRippoRasters, 12 },
    { "Rowf", "Rowf", kNpcRowfRasters, 22 },
    { "Minh T", "Minh T", kNpcMinhTRasters, 12 },
    { "Russ T", "Russ T", kNpcRussTRasters, 21 },
    { "Tayce T", "Tayce T", kNpcTayceTRasters, 22 },
    { "Fice T", "Fice T", kNpcFiceTRasters, 16 },
    { "Bartender", "Bartender", kNpcBartenderRasters, 11 },
    { "Chanterelle", "Chanterelle", kNpcChanterelleRasters, 13 },
    { "Rip Cheato", "Rip Cheato", kNpcRipCheatoRasters, 6 },
    { "Chuck Quizmo", "Chuck Quizmo", kNpcChuckQuizmoRasters, 41 },
    { "Merluvlee", "Merluvlee", kNpcMerluvleeRasters, 15 },
    { "Merlar", "Merlar", kNpcMerlarRasters, 8 },
    { "Merlow", "Merlow", kNpcMerlowRasters, 9 },
    { "Star Man", "Star Man", kNpcStarManRasters, 3 },
    { "Kolorado Wife", "Kolorado Wife", kNpcKoloradoWifeRasters, 7 },
    { "Koopa Koot", "Koopa Koot", kNpcKoopaKootRasters, 14 },
    { "Kolorado", "Kolorado", kNpcKoloradoRasters, 26 },
    { "Kolorado", "Battle Kolorado", kNpcBattleKoloradoRasters, 13 },
    { "Archeologist", "Archeologist", kNpcArcheologistRasters, 6 },
    { "Nomadimouse", "Nomadimouse", kNpcNomadimouseRasters, 4 },
    { "Merlee", "World Merlee", kNpcWorldMerleeRasters, 17 },
    { "Merlee", "Battle Merlee", kNpcBattleMerleeRasters, 2 },
    { "Disguised Moustafa", "Disguised Moustafa", kNpcDisguisedMoustafaRasters, 8 },
    { "Moustafa", "Moustafa", kNpcMoustafaRasters, 9 },
    { "Oaklie", "Oaklie", kNpcOaklieRasters, 15 },
    { "Bootler", "Bootler", kNpcBootlerRasters, 15 },
    { "Yakkey", "Yakkey", kNpcYakkeyRasters, 12 },
    { "Gourmet Guy", "Gourmet Guy", kNpcGourmetGuyRasters, 21 },
    { "Village Leader", "Village Leader", kNpcVillageLeaderRasters, 25 },
    { "Leaders Friend", "Leaders Friend", kNpcLeadersFriendRasters, 17 },
    { "Raphael Raven", "Raphael Raven", kNpcRaphaelRavenRasters, 34 },
    { "Tolielup", "Tolielup", kNpcTolielupRasters, 27 },
    { "Gate Flower", "Gate Flower", kNpcGateFlowerRasters, 11 },
    { "Petunia", "Petunia", kNpcPetuniaRasters, 20 },
    { "Posie", "Posie", kNpcPosieRasters, 13 },
    { "Lily", "Lily", kNpcLilyRasters, 18 },
    { "Rosie", "Rosie", kNpcRosieRasters, 11 },
    { "Sun", "Sun", kNpcSunRasters, 15 },
    { "Lakilulu", "Lakilulu", kNpcLakiluluRasters, 29 },
    { "Ninji", "Ninji", kNpcNinjiRasters, 6 },
    { "Mayor Penguin", "Mayor Penguin", kNpcMayorPenguinRasters, 26 },
    { "Mayor Penguin Wife", "Mayor Penguin Wife", kNpcMayorPenguinWifeRasters, 22 },
    { "Penguin Patrol", "Penguin Patrol", kNpcPenguinPatrolRasters, 14 },
    { "Herringway", "Herringway", kNpcHerringwayRasters, 21 },
    { "Merle", "Merle", kNpcMerleRasters, 11 },
    { "Star Rod", "Star Rod", kNpcStarRodRasters, 1 },
    { "Fire", "Fire", kNpcFireRasters, 3 },
    { "Coin", "Coin", kNpcCoinRasters, 10 },
    { "Parade Peach", "Parade Peach", kNpcParadePeachRasters, 44 },
    { "Parade Koopas", "Parade Koopas", kNpcParadeKoopasRasters, 30 },
    { "Parade Burnt Bowser", "Parade Burnt Bowser", kNpcParadeBurntBowserRasters, 18 },
    { "Parade Luigi", "Parade Luigi", kNpcParadeLuigiRasters, 18 },
    { "Parade Partner", "Parade Partner", kNpcParadePartnerRasters, 40 },
    { "Parade Yoshi", "Parade Yoshi", kNpcParadeYoshiRasters, 17 },
    { "Parade Kolorado", "Parade Kolorado", kNpcParadeKoloradoRasters, 16 },
    { "Parade Chick", "Parade Chick", kNpcParadeChickRasters, 12 },
    { "Parade Ice Show", "Parade Ice Show", kNpcParadeIceShowRasters, 38 },
    { "Parade Toad", "Parade Toad", kNpcParadeToadRasters, 30 },
    { "Parade Twirler", "Parade Twirler", kNpcParadeTwirlerRasters, 10 },
    { "Parade Drummer", "Parade Drummer", kNpcParadeDrummerRasters, 5 },
    { "Parade Banner", "Parade Banner", kNpcParadeBannerRasters, 9 },
    { "Parade Horn", "Parade Horn", kNpcParadeHornRasters, 5 },
    { "Parade Tubba", "Parade Tubba", kNpcParadeTubbaRasters, 6 },
    { "Parade Wizard", "Parade Wizard", kNpcParadeWizardRasters, 31 },
    { "Parade Mario", "Parade Mario", kNpcParadeMarioRasters, 21 },
    { "Parade Shy Guy", "Parade Shy Guy", kNpcParadeShyGuyRasters, 30 },
    { "Parade Twink", "Parade Twink", kNpcParadeTwinkRasters, 2 },
    { "Leaf", "Leaf", kNpcLeafRasters, 1 },
};
const int kNpcSpriteNameCount = 233;

const SpriteSheetName kPlayerSpriteNames[] = {
    { "Mario", "Mario1", kPlayerMario1Rasters, 87 },
    { "Mario", "Mario1 Back", nullptr, 0 },
    { "Mario", "Mario B1", kPlayerMarioB1Rasters, 65 },
    { "Mario", "Mario B2", kPlayerMarioB2Rasters, 46 },
    { "Mario", "Mario B3", kPlayerMarioB3Rasters, 15 },
    { "Mario", "Mario W1", kPlayerMarioW1Rasters, 57 },
    { "Mario", "Mario W1 Back", nullptr, 0 },
    { "Mario", "Mario W2", kPlayerMarioW2Rasters, 64 },
    { "Mario", "Mario W3", kPlayerMarioW3Rasters, 13 },
    { "Peach", "Peach1", kPlayerPeach1Rasters, 46 },
    { "Peach", "Peach1 Back", nullptr, 0 },
    { "Peach", "Peach2", kPlayerPeach2Rasters, 89 },
    { "Peach", "Peach3", kPlayerPeach3Rasters, 44 },
};
const int kPlayerSpriteNameCount = 13;

} // namespace Pm64
