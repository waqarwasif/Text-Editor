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
    return EditResult::InvalidText;
}
} // namespace

EditResult TextEditor::insertText(int line, int column, const mystring &text)
{
    const int count = doc_.getLineCount();
    if (line < 1 || line > count + 1)
        return EditResult::LineOutOfRange;

    const bool pastEnd = (line == count + 1);
    const mystring none;
    const mystring &base = pastEnd ? none : doc_.getLine(line);

    mystring updated;
    const TextProcessing::TextStatus status =
        TextProcessing::insertAt(base, column, text, updated);
    if (status != TextProcessing::TextStatus::Ok)
        return toEditResult(status);

    if (pastEnd)
        doc_.insertLine(line, updated);
    else
        doc_.modifyLine(line, std::move(updated));

    files_.markModified();
    return EditResult::Ok;
}

EditResult TextEditor::insertNewLine(int line, const mystring &text)
{
    if (line < 1 || line > doc_.getLineCount() + 1)
        return EditResult::LineOutOfRange;
    if (!TextProcessing::isValidLineText(text))
        return EditResult::InvalidText;

    doc_.insertLine(line, text);
    files_.markModified();
    return EditResult::Ok;
}

const char *TextEditor::describe(EditResult r)
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
