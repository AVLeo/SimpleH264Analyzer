#include "stdafx.h"
#include "Macroblock.h"
#include "PicParamSet.h"


CMacroblock::CMacroblock(UINT8 *pSODB, UINT32 offset)
{
	memset(this, 0, sizeof(CMacroblock));

	m_pSODB = pSODB;
	m_bypeOffset = offset / 8;
	m_bitOffset = offset % 8;
}


CMacroblock::~CMacroblock()
{
	if (m_pred_struct)
	{
		delete[] m_pred_struct;
		m_pred_struct = NULL;
	}
}

void CMacroblock::Set_paramaters(CPicParamSet *pps)
{
	m_pps_active = pps;
}

UINT32 CMacroblock::Parse_macroblock()
{
	UINT32 macroblockLength = 0;

	m_mb_type = Get_uev_code_num(m_pSODB, m_bypeOffset, m_bitOffset);

	if (m_mb_type == 25)
	{
		// To do: I-PCM mode...
	} 
	else if (m_mb_type == 0)
	{
		// Intra_NxN mode...
		if (m_pps_active->Get_transform_8x8_mode_flag())
		{
			m_transform_size_8x8_flag = Get_bit_at_position(m_pSODB, m_bypeOffset, m_bitOffset);
		}

		// Get prediction-block num...
		if (m_transform_size_8x8_flag)
		{
			// Using intra_8x8
			m_pred_struct = new IntraPredStruct[4];	
			for (int luma8x8BlkIdx = 0; luma8x8BlkIdx < 4; luma8x8BlkIdx++)
			{
				m_pred_struct[luma8x8BlkIdx].block_mode = 1;
				m_pred_struct[luma8x8BlkIdx].prev_intra_pred_mode_flag = Get_bit_at_position(m_pSODB, m_bypeOffset, m_bitOffset);
				if (!m_pred_struct[luma8x8BlkIdx].prev_intra_pred_mode_flag)
				{
					m_pred_struct[luma8x8BlkIdx].rem_intra_pred_mode = Get_uint_code_num(m_pSODB, m_bypeOffset, m_bitOffset, 3);
				}
			}
		} 
		else
		{
			// Using intra_4x4
			m_pred_struct = new IntraPredStruct[16];
			for (int luma4x4BlkIdx = 0; luma4x4BlkIdx < 16; luma4x4BlkIdx++)
			{
				m_pred_struct[luma4x4BlkIdx].block_mode = 0;
				m_pred_struct[luma4x4BlkIdx].prev_intra_pred_mode_flag = Get_bit_at_position(m_pSODB, m_bypeOffset, m_bitOffset);
				if (!m_pred_struct[luma4x4BlkIdx].prev_intra_pred_mode_flag)
				{
					m_pred_struct[luma4x4BlkIdx].rem_intra_pred_mode = Get_uint_code_num(m_pSODB, m_bypeOffset, m_bitOffset, 3);
				}
			}
		}

		// To do: intra_chroma_pred_mode
	}
	else
	{
		// To do: Intra_16x16 mode
	}

	return macroblockLength;
}
