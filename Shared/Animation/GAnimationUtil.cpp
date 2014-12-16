#include "GAnimationUtil.h"

GAnimClip* GAnimationUtil::LoadAnimClip(const char* i_fileName)
{
	char* filepath = (char*)malloc(128);
	sprintf(filepath, "data/Anims/%s", i_fileName);
	FILE* file = fopen(filepath, "r+b");

	GAnimClip* clip = new GAnimClip();
	clip->DeSerialize(file);
	sprintf(clip->m_Name, "%s", i_fileName);
	//clean up hack
	free(filepath);
	return clip;
}

void GAnimationUtil::Evaluate_AnimClip(GSkeletonInstance* io_instance, GAnimClip* i_clip, float i_time)
{
	for (int i = 0; i < i_clip->m_Tracks.Count(); i++)
	{
		u32 keyOne, keyTwo;
		i_clip->m_Tracks[i].GetKeyRotationIndices(keyOne, keyTwo, i_time);

		if (keyOne != keyTwo)
		{
			float alpha = (i_time - i_clip->m_Tracks[i].m_RotKeys[keyOne].m_Time) / (i_clip->m_Tracks[i].m_RotKeys[keyTwo].m_Time - i_clip->m_Tracks[i].m_RotKeys[keyOne].m_Time);
			io_instance->m_Bones[i_clip->m_TrackToBone[i]].m_LocalRot = io_instance->m_Bones[i_clip->m_TrackToBone[i]].m_LocalRot.Slerp(i_clip->m_Tracks[i].m_RotKeys[keyOne].m_Rotation, i_clip->m_Tracks[i].m_RotKeys[keyTwo].m_Rotation, alpha);
		}
		else
			io_instance->m_Bones[i_clip->m_TrackToBone[i]].m_LocalRot = i_clip->m_Tracks[i].m_RotKeys[keyOne].m_Rotation;

		// we can probably just use key one, key two, and alpha if we keep our sampling method...until then, search again.
		i_clip->m_Tracks[i].GetKeyTranslationIndices(keyOne, keyTwo, i_time);
		if (keyOne != keyTwo)
		{
			// potentially smooth step here.
			float alpha = GMath::SmoothStep(i_clip->m_Tracks[i].m_TranslationKeys[keyOne].m_Time, i_clip->m_Tracks[i].m_TranslationKeys[keyTwo].m_Time, i_time);
			//float alpha = (i_time - m_Tracks[i].m_TranslationKeys[keyOne].m_Time) / (m_Tracks[i].m_TranslationKeys[keyTwo].m_Time - m_Tracks[i].m_TranslationKeys[keyOne].m_Time);
			io_instance->m_Bones[i_clip->m_TrackToBone[i]].m_LocalTranslation = GVector3::Lerp(i_clip->m_Tracks[i].m_TranslationKeys[keyOne].m_Translation, i_clip->m_Tracks[i].m_TranslationKeys[keyTwo].m_Translation, alpha);
		}
		else
			io_instance->m_Bones[i_clip->m_TrackToBone[i]].m_LocalTranslation = i_clip->m_Tracks[i].m_TranslationKeys[keyOne].m_Translation;
	}
}

// linear blend for the time being...blah.
void GAnimationUtil::Evaluate_AnimClipBlend(GSkeletonInstance* io_instance, GAnimClip* i_intoClip, GAnimClip* i_outOfClip, float i_intoTime, float i_outOfTime, float i_intoAlpha)
{
	// DO THE FIRST CLIP...
	for (int i = 0; i < i_intoClip->m_Tracks.Count(); i++)
	{
		u32 keyOne, keyTwo;
		i_intoClip->m_Tracks[i].GetKeyRotationIndices(keyOne, keyTwo, i_intoTime);

		if (keyOne != keyTwo)
		{
			float alpha = (i_intoTime - i_intoClip->m_Tracks[i].m_RotKeys[keyOne].m_Time) / (i_intoClip->m_Tracks[i].m_RotKeys[keyTwo].m_Time - i_intoClip->m_Tracks[i].m_RotKeys[keyOne].m_Time);
			io_instance->m_Bones[i_intoClip->m_TrackToBone[i]].m_LocalRot.Slerp(i_intoClip->m_Tracks[i].m_RotKeys[keyOne].m_Rotation, i_intoClip->m_Tracks[i].m_RotKeys[keyTwo].m_Rotation, alpha);
		}
		else
			io_instance->m_Bones[i_intoClip->m_TrackToBone[i]].m_LocalRot = i_intoClip->m_Tracks[i].m_RotKeys[keyOne].m_Rotation;

		// we can probably just use key one, key two, and alpha if we keep our sampling method...until then, search again.
		i_intoClip->m_Tracks[i].GetKeyTranslationIndices(keyOne, keyTwo, i_intoTime);
		if (keyOne != keyTwo)
		{
			// potentially smooth step here.
			float alpha = GMath::SmoothStep(i_intoClip->m_Tracks[i].m_TranslationKeys[keyOne].m_Time, i_intoClip->m_Tracks[i].m_TranslationKeys[keyTwo].m_Time, i_intoTime);
			//float alpha = (i_time - m_Tracks[i].m_TranslationKeys[keyOne].m_Time) / (m_Tracks[i].m_TranslationKeys[keyTwo].m_Time - m_Tracks[i].m_TranslationKeys[keyOne].m_Time);
			io_instance->m_Bones[i_intoClip->m_TrackToBone[i]].m_LocalTranslation = GVector3::Lerp(i_intoClip->m_Tracks[i].m_TranslationKeys[keyOne].m_Translation, i_intoClip->m_Tracks[i].m_TranslationKeys[keyTwo].m_Translation, alpha);
			io_instance->m_Bones[i_intoClip->m_TrackToBone[i]].m_LocalTranslation *= i_intoAlpha;
		}
		else
			io_instance->m_Bones[i_intoClip->m_TrackToBone[i]].m_LocalTranslation = i_intoClip->m_Tracks[i].m_TranslationKeys[keyOne].m_Translation * i_intoAlpha;
	}

	float i_outOfAlpha = (1.0f - i_intoAlpha);
	for (int i = 0; i < i_outOfClip->m_Tracks.Count(); i++)
	{
		u32 keyOne, keyTwo;
		i_outOfClip->m_Tracks[i].GetKeyRotationIndices(keyOne, keyTwo, i_outOfTime);

		if (keyOne != keyTwo)
		{
			float alpha = (i_outOfTime - i_outOfClip->m_Tracks[i].m_RotKeys[keyOne].m_Time) / (i_outOfClip->m_Tracks[i].m_RotKeys[keyTwo].m_Time - i_outOfClip->m_Tracks[i].m_RotKeys[keyOne].m_Time);
			GQuat result;
			result.Slerp(i_outOfClip->m_Tracks[i].m_RotKeys[keyOne].m_Rotation, i_outOfClip->m_Tracks[i].m_RotKeys[keyTwo].m_Rotation, alpha);
			io_instance->m_Bones[i_outOfClip->m_TrackToBone[i]].m_LocalRot.Slerp(result, io_instance->m_Bones[i_outOfClip->m_TrackToBone[i]].m_LocalRot, i_intoAlpha);
		}
		else
			io_instance->m_Bones[i_outOfClip->m_TrackToBone[i]].m_LocalRot *= i_outOfClip->m_Tracks[i].m_RotKeys[keyOne].m_Rotation * i_outOfAlpha;

		// we can probably just use key one, key two, and alpha if we keep our sampling method...until then, search again.
		i_outOfClip->m_Tracks[i].GetKeyTranslationIndices(keyOne, keyTwo, i_outOfTime);
		if (keyOne != keyTwo)
		{
			// potentially smooth step here.
			float alpha = GMath::SmoothStep(i_outOfClip->m_Tracks[i].m_TranslationKeys[keyOne].m_Time, i_outOfClip->m_Tracks[i].m_TranslationKeys[keyTwo].m_Time, i_outOfTime);
			//float alpha = (i_time - m_Tracks[i].m_TranslationKeys[keyOne].m_Time) / (m_Tracks[i].m_TranslationKeys[keyTwo].m_Time - m_Tracks[i].m_TranslationKeys[keyOne].m_Time);
			io_instance->m_Bones[i_outOfClip->m_TrackToBone[i]].m_LocalTranslation += i_outOfAlpha * GVector3::Lerp(i_outOfClip->m_Tracks[i].m_TranslationKeys[keyOne].m_Translation, i_outOfClip->m_Tracks[i].m_TranslationKeys[keyTwo].m_Translation, alpha);
		}
		else
			io_instance->m_Bones[i_outOfClip->m_TrackToBone[i]].m_LocalTranslation += i_outOfAlpha * i_outOfClip->m_Tracks[i].m_TranslationKeys[keyOne].m_Translation * i_outOfAlpha;
	}
}