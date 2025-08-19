# RE7 UObjectHook Configuration

## Overview
This directory contains persistent UObjectHook attachment configurations for Resident Evil 7.

## Files
- `weapon_attachments.json` - Weapon to right controller attachments
- `utility_attachments.json` - Utility items to left controller attachments
- `attachment_state.json` - Complete attachment state persistence

## Attachment Strategy
- **Weapon → Right Controller**: Primary weapon, combat items
- **Utility → Left Controller**: Inventory, flashlight, utility items

## Validation Status
- [ ] Weapon attachments configured and tested
- [ ] Utility attachments configured and tested
- [ ] All attachments persist across sessions
- [ ] Input alignment matches attachment configuration

## Notes
- RE7 uses first-person camera system
- No third-person to first-person conversion required
- Focus on weapon handling and utility item accessibility
