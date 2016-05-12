#ifndef BIT_TOOLS_H_
#define BIT_TOOLS_H_

//! @}

/*! \name Bit-Field Handling
 */
//! @{

/*! \brief Reads the bits of a value specified by a given bit-mask.
 *
 * \param value Value to read bits from.
 * \param mask  Bit-mask indicating bits to read.
 *
 * \return Read bits.
 */
#define Rd_bits( value, mask)        ((value) & (mask))

/*! \brief Writes the bits of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue  C lvalue to write bits to.
 * \param mask    Bit-mask indicating bits to write.
 * \param bits    Bits to write.
 *
 * \return Resulting value with written bits.
 */
#define Wr_bits(lvalue, mask, bits)  ((lvalue) = ((lvalue) & ~(mask)) |\
                                                 ((bits  ) &  (mask)))

/*! \brief Tests the bits of a value specified by a given bit-mask.
 *
 * \param value Value of which to test bits.
 * \param mask  Bit-mask indicating bits to test.
 *
 * \return \c 1 if at least one of the tested bits is set, else \c 0.
 */
#define Tst_bits( value, mask)  (Rd_bits(value, mask) != 0)

/*! \brief Clears the bits of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue  C lvalue of which to clear bits.
 * \param mask    Bit-mask indicating bits to clear.
 *
 * \return Resulting value with cleared bits.
 */
#define Clr_bits(lvalue, mask)  ((lvalue) &= ~(mask))

/*! \brief Sets the bits of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue  C lvalue of which to set bits.
 * \param mask    Bit-mask indicating bits to set.
 *
 * \return Resulting value with set bits.
 */
#define Set_bits(lvalue, mask)  ((lvalue) |=  (mask))

/*! \brief Toggles the bits of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue  C lvalue of which to toggle bits.
 * \param mask    Bit-mask indicating bits to toggle.
 *
 * \return Resulting value with toggled bits.
 */
#define Tgl_bits(lvalue, mask)  ((lvalue) ^=  (mask))

/*! \brief Reads the bit-field of a value specified by a given bit-mask.
 *
 * \param value Value to read a bit-field from.
 * \param mask  Bit-mask indicating the bit-field to read.
 *
 * \return Read bit-field.
 */
#define Rd_bitfield( value, mask)           (Rd_bits( value, mask) >> ctz(mask))

/*! \brief Writes the bit-field of a C lvalue specified by a given bit-mask.
 *
 * \param lvalue    C lvalue to write a bit-field to.
 * \param mask      Bit-mask indicating the bit-field to write.
 * \param bitfield  Bit-field to write.
 *
 * \return Resulting value with written bit-field.
 */
#define Wr_bitfield(lvalue, mask, bitfield) (Wr_bits(lvalue, mask, (U32)(bitfield) << ctz(mask)))

//! @}

#endif /* BIT_TOOLS_H_ */