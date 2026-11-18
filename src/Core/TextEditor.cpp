#include "Core/TextEditor.h"
#include "Utils/TextProcessing.h"

#include <utility>

namespace
{
EditResult toEditResult(TextProcessing::TextStatus status)
{
    using TextProcessing::TextStatus;
    switch (status)
    {
    case TextStatus::Ok:               return EditResult::Ok;
    case TextStatus::EmptyText:        return EditResult::NothingToInsert;
    case TextStatus::InvalidCharacter: return EditResult::InvalidText;
    case TextStatus::ColumnOutOfRange: return EditResult::ColumnOutOfRange;
    }
    return EditResult::InvalidText; // unreachable: every status is handled above
}
} // namespace

EditResult TextEditor::insertText(int line, int column, const std::string &text)
{
    const int count = doc_.getLineCount();
    if (line < 1 || line > count + 1)
        return EditResult::LineOutOfRange;

    // Past the last line: treat the missing line as an empty string, so
    // insertAt itself enforces "column must be 1" and the text checks.
    const bool pastEnd = (line == count + 1);
    const std::string none;
    const std::string &base = pastEnd ? none : doc_.getLine(line); // no copy

    std::string updated;
    const TextProcessing::TextStatus status =
        TextProcessing::insertAt(base, column, text, updated);
    if (status != TextProcessing::TextStatus::Ok)
        return toEditResult(status); // nothing touched, not marked modified

    // `base` is not used past this point (modifyLine overwrites that node).
    if (pastEnd)
        doc_.insertLine(line, updated);
    else
        doc_.modifyLine(line, std::move(updated)); // moved in, not copied

    files_.markModified();
    return EditResult::Ok;
}

EditResult TextEditor::insertNewLine(int line, const std::string &text)
{
    if (line < 1 || line > doc_.getLineCount() + 1)
        return EditResult::LineOutOfRange;
    if (!TextProcessing::isValidLineText(text))
        return EditResult::InvalidText; // an empty line is allowed: that is the point

    doc_.insertLine(line, text);
    files_.markModified();
    return EditResult::Ok;
}

std::string TextEditor::describe(EditResult r)
{
    switch (r)
    {
    case EditResult::Ok:               return "Success.";
    case EditResult::NothingToInsert:  return "Nothing to insert (the text is empty).";
    case EditResult::InvalidText:      return "Text cannot contain line breaks or NUL characters.";
    case EditResult::LineOutOfRange:   return "Line number is out of range.";
    case EditResult::ColumnOutOfRange: return "Column is out of range.";
    }
    return "Unknown error.";
}