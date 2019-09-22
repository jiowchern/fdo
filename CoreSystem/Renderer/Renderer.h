#pragma once

struct IRendererResource {
	virtual ~IRendererResource(){};
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;
	virtual void OnRestoreDevice() = 0;
};