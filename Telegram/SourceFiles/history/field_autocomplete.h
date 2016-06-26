/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

In addition, as a special exception, the copyright holders give permission
to link the code of portions of this program with the OpenSSL library.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014-2016 John Preston, https://desktop.telegram.org
*/
#pragma once

#include "ui/twidget.h"
#include "ui/boxshadow.h"

namespace internal {

using MentionRows = QList<UserData*>;
using HashtagRows = QList<QString>;
using BotCommandRows = QList<QPair<UserData*, const BotCommand*>>;

class FieldAutocompleteInner;

} // namespace internal

class FieldAutocomplete final : public TWidget {
	Q_OBJECT

public:

	FieldAutocomplete(QWidget *parent);

	void fastHide();

	bool clearFilteredBotCommands();
	void showFiltered(PeerData *peer, QString query, bool addInlineBots);
	void showStickers(EmojiPtr emoji);
	void setBoundings(QRect boundings);

	void step_appearance(float64 ms, bool timer);

	const QString &filter() const;
	ChatData *chat() const;
	ChannelData *channel() const;
	UserData *user() const;

	int32 innerTop();
	int32 innerBottom();

	bool eventFilter(QObject *obj, QEvent *e) override;

	enum class ChooseMethod {
		ByEnter,
		ByTab,
		ByClick,
	};
	bool chooseSelected(ChooseMethod method) const;

	bool stickersShown() const {
		return !_srows.isEmpty();
	}

	bool overlaps(const QRect &globalRect) {
		if (isHidden() || !testAttribute(Qt::WA_OpaquePaintEvent)) return false;

		return rect().contains(QRect(mapFromGlobal(globalRect.topLeft()), globalRect.size()));
	}

	~FieldAutocomplete();

signals:

	void mentionChosen(UserData *user, FieldAutocomplete::ChooseMethod method) const;
	void hashtagChosen(QString hashtag, FieldAutocomplete::ChooseMethod method) const;
	void botCommandChosen(QString command, FieldAutocomplete::ChooseMethod method) const;
	void stickerChosen(DocumentData *sticker, FieldAutocomplete::ChooseMethod method) const;

	void moderateKeyActivate(int key, bool *outHandled) const;

public slots:

	void hideStart();
	void hideFinish();

	void showStart();

private:

	void paintEvent(QPaintEvent *e) override;

	void updateFiltered(bool resetScroll = false);
	void recount(bool resetScroll = false);

	QPixmap _cache;
	internal::MentionRows _mrows;
	internal::HashtagRows _hrows;
	internal::BotCommandRows _brows;
	StickerPack _srows;

	void rowsUpdated(const internal::MentionRows &mrows, const internal::HashtagRows &hrows, const internal::BotCommandRows &brows, const StickerPack &srows, bool resetScroll);

	ChildWidget<ScrollArea> _scroll;
	ChildWidget<internal::FieldAutocompleteInner> _inner;

	ChatData *_chat;
	UserData *_user;
	ChannelData *_channel;
	EmojiPtr _emoji;
	enum class Type {
		Mentions,
		Hashtags,
		BotCommands,
		Stickers,
	};
	Type _type = Type::Mentions;
	QString _filter;
	QRect _boundings;
	bool _addInlineBots;

	int32 _width, _height;
	bool _hiding;

	anim::fvalue a_opacity;
	Animation _a_appearance;

	QTimer _hideTimer;

	BoxShadow _shadow;
	friend class internal::FieldAutocompleteInner;

};

namespace internal {

class FieldAutocompleteInner final : public TWidget {
	Q_OBJECT

public:

	FieldAutocompleteInner(FieldAutocomplete *parent, MentionRows *mrows, HashtagRows *hrows, BotCommandRows *brows, StickerPack *srows);

	void clearSel(bool hidden = false);
	bool moveSel(int key);
	bool chooseSelected(FieldAutocomplete::ChooseMethod method) const;

	void setRecentInlineBotsInRows(int32 bots);

signals:

	void mentionChosen(UserData *user, FieldAutocomplete::ChooseMethod method) const;
	void hashtagChosen(QString hashtag, FieldAutocomplete::ChooseMethod method) const;
	void botCommandChosen(QString command, FieldAutocomplete::ChooseMethod method) const;
	void stickerChosen(DocumentData *sticker, FieldAutocomplete::ChooseMethod method) const;
	void mustScrollTo(int scrollToTop, int scrollToBottom);

public slots:

	void onParentGeometryChanged();
	void onUpdateSelected(bool force = false);
	void onPreview();

private:

	void paintEvent(QPaintEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;

	void enterEvent(QEvent *e) override;
	void leaveEvent(QEvent *e) override;

	void mousePressEvent(QMouseEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;

	void updateSelectedRow();
	void setSel(int sel, bool scroll = false);

	FieldAutocomplete *_parent;
	MentionRows *_mrows;
	HashtagRows *_hrows;
	BotCommandRows *_brows;
	StickerPack *_srows;
	int32 _stickersPerRow, _recentInlineBotsInRows;
	int32 _sel, _down;
	bool _mouseSel;
	QPoint _mousePos;

	bool _overDelete;

	bool _previewShown;

	QTimer _previewTimer;
};

} // namespace internal
